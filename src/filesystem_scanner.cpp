#include "filesystem_scanner.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <queue>

filesystem_scanner::filesystem_scanner(){}

filesystem_scanner::grouped_by_size filesystem_scanner::scan(const scan_task& task)
{
    dir_filters dir_f = create_dir_filters(task.scanning_level, task.scanning_excluded_paths);
    file_filters file_f = create_file_filters(task.scanning_file_min_size, task.scanning_masks);

    auto all_files = all_accepted_files(task.scanning_paths, dir_f, file_f);

    remove_uniq_sized_files(all_files);

    return all_files;
}

void filesystem_scanner::handle_dir(std::queue<scan_dir> &result,
                                    const dir_filters& dir_f,
                                    const scan_dir dir)
{
    auto predicate = [dir](const dir_filter& filter) {return filter(dir);};
    if(std::all_of(dir_f.begin(), dir_f.end(), predicate))
        result.push(dir);
}

void filesystem_scanner::handle_file(grouped_by_size& result,
                                     const file_filters& file_f,
                                     const bfs::path& path)
{
    auto predicate = [path](const file_filter& filter) {return filter(path);};
    if(std::all_of(file_f.begin(), file_f.end(), predicate))
    {
        size_t size = bfs::file_size(path);
        auto found_iter = result.find(size);
        if(found_iter == result.end())
        {
            uniq_paths uniq_paths;
            uniq_paths.insert(path);
            result[size] = uniq_paths;
        }
        else
            found_iter->second.insert(path);
    }
}
void filesystem_scanner::remove_uniq_sized_files(grouped_by_size& files)
{
    auto iter = files.begin();
    while(iter != files.end())
        if(iter->second.size() < 2)
            iter = files.erase(iter);
        else
            ++iter;
}

filesystem_scanner::grouped_by_size filesystem_scanner::all_accepted_files(
        const paths& included,
        const dir_filters& dir_f,
        const file_filters& file_f)
{
    grouped_by_size result;

    std::queue<scan_dir> to_scan_dirs;
    for(const auto& dir : included)
        to_scan_dirs.push(std::make_pair(dir, 0));

    while(!to_scan_dirs.empty())
    {
        scan_dir current_scan_dir = to_scan_dirs.front();
        to_scan_dirs.pop();

        bfs::path current_dir = current_scan_dir.first;
        if(!bfs::exists(current_dir))
            continue;

        bsys::error_code error;
        bfs::directory_iterator it(current_dir, error);
        if(error.failed())
            continue;

        bfs::directory_iterator end;
        for(;it != end; ++it)
        {
            if(bfs::is_directory(*it))
                handle_dir(to_scan_dirs, dir_f, std::make_pair(*it, current_scan_dir.second + 1));
            else if(bfs::is_regular_file(*it))
            {
                bfs::file_status stat = bfs::symlink_status(*it);
                if(stat.type() == bfs::file_type::symlink_file)
                {
                    bfs::path rel_path = bfs::read_symlink(*it);
                    bfs::path base_path = it->path().parent_path();
                    bfs::path abs_path = bfs::canonical(rel_path, base_path);

                    if(bfs::exists(abs_path))
                        handle_file(result, file_f, abs_path);
                }
                else
                    handle_file(result, file_f, *it);
            }
        }
    }

    return result;
}

filesystem_scanner::file_filters filesystem_scanner::create_file_filters(
        const std::optional<size_t>& scanning_file_min_size,
        const std::vector<std::string>& scanning_masks)
{
    file_filters f;

    if(scanning_file_min_size.has_value())
        f.emplace_back(file_min_size_filter(scanning_file_min_size.value()));
    else
        f.emplace_back(file_min_size_filter(1));

    if(!scanning_masks.empty())
        f.emplace_back(file_accepted_masks(scanning_masks));

    return f;
}


filesystem_scanner::file_filter filesystem_scanner::file_min_size_filter(size_t file_min_size)
{
    return [file_min_size](const bfs::path& path) {
        return bfs::file_size(path) >= file_min_size;
    };
}

filesystem_scanner::file_filter filesystem_scanner::file_accepted_masks(
        const std::vector<std::string>& scanning_masks)
{
    std::vector<boost::regex> regex_masks;
    for(const auto& str_mask : scanning_masks)
        regex_masks.emplace_back(boost::regex(str_mask));

    return [regex_masks](const bfs::path& path) {
        std::string file_name = path.filename().string();

        auto predicate = [file_name](const boost::regex& regex) {
            return boost::regex_match(file_name, regex);
        };
        return std::any_of(regex_masks.begin(), regex_masks.end(), predicate);
    };
}

filesystem_scanner::dir_filters filesystem_scanner::create_dir_filters(
        const std::optional<size_t>& scanning_level,
        const paths& excluded)
{
    dir_filters filters;

    if(scanning_level.has_value())
        filters.push_back(dir_level_filter(scanning_level.value()));

    if(!excluded.empty())
        filters.push_back(dir_excluded_filter(excluded));

    return filters;
}

filesystem_scanner::dir_filter filesystem_scanner::dir_level_filter(size_t level)
{
    return [level](const scan_dir& description) {
        return description.second <= level;
    };
}

filesystem_scanner::dir_filter filesystem_scanner::dir_excluded_filter(paths excluded)
{
    return [excluded](const scan_dir& description) {

        if(excluded.empty())
            return true;

        const auto& dir = description.first;
        auto predicate = [dir](const bfs::path& ex)
        {
            if(dir == ex)
                return true;
            else
            {
                //check relative
                return false;
            }
        };

        auto iter = std::find_if(excluded.begin(), excluded.end(), predicate);
        return iter == excluded.end();
    };
}
