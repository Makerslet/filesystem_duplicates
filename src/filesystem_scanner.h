#ifndef FILESYSTEM_SCANNER_H
#define FILESYSTEM_SCANNER_H

#include <boost/filesystem/path.hpp>
#include <functional>

class filesystem_scanner
{
public:
    using paths = std::vector<boost::filesystem::path>;
    using file_filter = std::function<bool(const boost::filesystem::path&)>;
    using filters = std::vector<file_filter>;
    using grouped_by_size = std::unordered_map<size_t, paths>;

    filesystem_scanner();
    grouped_by_size scan(
            const paths& included, const paths& excluded,
            const std::optional<size_t>& scanning_level,
            const std::optional<size_t>& scanning_file_min_size,
            const std::vector<std::string>& scanning_masks);

private:
    std::unordered_map<size_t, paths> all_accepted_files(
            const paths& included, const paths& excluded,
            const std::optional<size_t>& scanning_level,
            const filters& f);
    filters create_filters(const std::optional<size_t> &scanning_file_min_size,
            const std::vector<std::string> &scanning_masks);
    file_filter file_min_size_filter(size_t file_min_size);
    file_filter file_accepted_masks(const std::vector<std::string>& scanning_masks);
};

#endif // FILESYSTEM_SCANNER_H
