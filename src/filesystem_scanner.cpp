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
    auto iter = all_files.begin();
    while(iter != all_files.end())
        if(iter->second.size() == 1)
            iter = all_files.erase(iter);

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
                                     const boost::filesystem::path& path)
{
    auto predicate = [path](const file_filter& filter) {return filter(path);};
    if(std::all_of(file_f.begin(), file_f.end(), predicate))
    {
        size_t size = boost::filesystem::file_size(path);
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
        const scan_dir& current_scan_dir = to_scan_dirs.front();
        to_scan_dirs.pop();

        const boost::filesystem::path& current_dir = current_scan_dir.first;
        for(const boost::filesystem::directory_entry& nested :
            boost::filesystem::directory_iterator(current_dir))
        {
            if(boost::filesystem::is_directory(nested))
                handle_dir(to_scan_dirs, dir_f, std::make_pair(nested, current_scan_dir.second + 1));
            else if(boost::filesystem::is_symlink(nested))
            {
                auto resolved = boost::filesystem::read_symlink(nested);
                if(!resolved.empty())
                    handle_file(result, file_f, nested);
            }
            else if(boost::filesystem::is_regular_file(nested))
                handle_file(result, file_f, nested);
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

    if(!scanning_masks.empty())
        f.emplace_back(file_accepted_masks(scanning_masks));

    return f;
}


filesystem_scanner::file_filter filesystem_scanner::file_min_size_filter(size_t file_min_size)
{
    return [file_min_size](const boost::filesystem::path& path) {
        return boost::filesystem::file_size(path) >= file_min_size;
    };
}

filesystem_scanner::file_filter filesystem_scanner::file_accepted_masks(
        const std::vector<std::string>& scanning_masks)
{
    std::vector<boost::regex> regex_masks;
    for(const auto& str_mask : scanning_masks)
        regex_masks.emplace_back(boost::regex(str_mask));

    return [regex_masks](const boost::filesystem::path& path) {
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
    return [excluded](const scan_dir& description) mutable {

        if(excluded.empty())
            return true;

        const auto& dir = description.first;
        auto iter = std::remove(excluded.begin(), excluded.end(), dir);

        if(iter == excluded.end())
            return true;
        else
        {
            excluded.erase(iter, excluded.end());
            return false;
        }
    };
}
