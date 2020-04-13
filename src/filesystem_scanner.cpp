#include "filesystem_scanner.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <queue>

filesystem_scanner::filesystem_scanner(){}

filesystem_scanner::grouped_by_size filesystem_scanner::scan(const paths& included, const paths& excluded,
        const std::optional<size_t> &scanning_level,
        const std::optional<size_t> &scanning_file_min_size,
        const std::vector<std::string> &scanning_masks
        )
{
    filters f = create_filters(scanning_file_min_size, scanning_masks);
    auto all_files = all_accepted_files(included, excluded, scanning_level, f);

    auto it = std::remove_if(all_files.begin(), all_files.end(), [](const paths& p) {return p.size() > 1;});
    all_files.erase(it, all_files.end());

    return all_files;
}

std::unordered_map<size_t, filesystem_scanner::paths> filesystem_scanner::all_accepted_files(
        const paths& included, const paths& excluded,
        const std::optional<size_t>& scanning_level,
        const filters& f)
{
    std::unordered_map<size_t, filesystem_scanner::paths> result;
    return result;
}

filesystem_scanner::filters filesystem_scanner::create_filters(
        const std::optional<size_t>& scanning_file_min_size,
        const std::vector<std::string>& scanning_masks)
{
    filters f;

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
