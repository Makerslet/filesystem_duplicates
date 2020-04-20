#include "filters.h"

#include <boost/filesystem/operations.hpp>
#include <boost/regex.hpp>

file_filter file_filter_creator::file_min_size_filter(size_t file_min_size)
{
    return [file_min_size](const bfs::path& path) {
        return bfs::file_size(path) >= file_min_size;
    };
}

file_filter file_filter_creator::file_accepted_masks_filter(const masks& scanning_masks)
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

dir_filter dir_filter_creator::dir_level_filter(size_t level)
{
    return [level](const scan_dir& description) {
        return description.second <= level;
    };
}

dir_filter dir_filter_creator::dir_excluded_filter(paths excluded)
{
    return [excluded](const scan_dir& description) {

        if(excluded.empty())
            return true;

        const auto& dir = description.first;
        auto predicate = [dir](const bfs::path& ex)
        {
            if(dir == ex)
                return false;
            else
            {
                auto dir_string = dir.string();
                auto ex_string = ex.string();

                if(dir_string.find(ex_string) == std::string::npos)
                    return true;
                else
                    return false;
            }
        };

        auto iter = std::find_if(excluded.begin(), excluded.end(), predicate);
        return iter == excluded.end();
    };
}
