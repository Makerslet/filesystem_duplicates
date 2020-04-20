#include "filesystem_scanner.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <queue>

filesystem_scanner::filesystem_scanner(
        const paths &scanning_excluded,
        std::optional<size_t> scanning_level,
        std::optional<size_t> scanning_file_min_size,
        std::vector<std::string> scanning_masks) :
    _excluded(scanning_excluded)
{
    _dirs_f = create_dir_filters(scanning_level);
    _files_f = create_file_filters(scanning_file_min_size, scanning_masks);
}

grouped_by_size filesystem_scanner::scan(const paths &included)
{
    auto to_scan_paths = pre_check(included);
    auto all_files = all_accepted_files(to_scan_paths);

    remove_uniq_sized_files(all_files);

    return all_files;
}

paths filesystem_scanner::pre_check(const paths& included)
{
    paths result;

    for(const bfs::path& in_path : included)
    {
        bool need_add = true;

        for(const bfs::path& ex_path: _excluded)
        {
            auto in_string = in_path.string();
            auto ex_string = ex_path.string();

            if(in_string.find(ex_string) != std::string::npos)
            {
                need_add = false;
                break;
            }
        }

        if(need_add)
            result.push_back(in_path);
    }

    return result;
}

void filesystem_scanner::handle_dir(std::queue<scan_dir> &result,
                                    const scan_dir dir)
{
    auto predicate = [dir](const dir_filter& filter) {return filter(dir);};
    if(std::all_of(_dirs_f.begin(), _dirs_f.end(), predicate))
        result.push(dir);
}

void filesystem_scanner::handle_file(grouped_by_size& result,
                                     const bfs::path& path)
{
    auto predicate = [path](const file_filter& filter) {return filter(path);};
    if(std::all_of(_files_f.begin(), _files_f.end(), predicate))
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

grouped_by_size filesystem_scanner::all_accepted_files(
        const paths& included)
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
        if(error.value() != bsys::errc::success)
            continue;

        bfs::directory_iterator end;
        for(;it != end; ++it)
        {
            if(bfs::is_directory(*it))
                handle_dir(to_scan_dirs, std::make_pair(*it, current_scan_dir.second + 1));
            else if(bfs::is_regular_file(*it))
            {
                bfs::file_status stat = bfs::symlink_status(*it);
                if(stat.type() == bfs::file_type::symlink_file)
                {
                    bfs::path rel_path = bfs::read_symlink(*it);
                    bfs::path base_path = it->path().parent_path();
                    bfs::path abs_path = bfs::canonical(rel_path, base_path);

                    if(bfs::exists(abs_path))
                        handle_file(result, abs_path);
                }
                else
                    handle_file(result, *it);
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

    size_t min_size_file = 1;
    if(scanning_file_min_size.has_value())
        min_size_file = scanning_file_min_size.value();
    f.emplace_back(file_filter_creator::file_min_size_filter(min_size_file));

    if(!scanning_masks.empty())
        f.emplace_back(file_filter_creator::file_accepted_masks_filter(scanning_masks));

    return f;
}

filesystem_scanner::dir_filters filesystem_scanner::create_dir_filters(const std::optional<size_t>& scanning_level)
{
    dir_filters filters;

    if(scanning_level.has_value())
        filters.push_back(dir_filter_creator::dir_level_filter(scanning_level.value()));

    if(!_excluded.empty())
        filters.push_back(dir_filter_creator::dir_excluded_filter(_excluded));

    return filters;
}
