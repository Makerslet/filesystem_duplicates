#ifndef FILESYSTEM_SCANNER_H
#define FILESYSTEM_SCANNER_H

#include <boost/filesystem/path.hpp>
#include <functional>

class filesystem_scanner
{
public:
    using scan_result = std::pair<boost::filesystem::path, size_t>;
    using results = std::vector<scan_result>;

    using paths = std::vector<boost::filesystem::path>;
    using file_filter = std::function<bool(const boost::filesystem::path&)>;
    using filters = std::vector<file_filter>;

    filesystem_scanner();
    results scan(
            const paths& included, const paths& excluded,
            std::optional<size_t> scanning_level,
            std::optional<size_t> scanning_file_min_size,
            std::vector<std::string> scanning_masks);

private:
    filters create_filters(const std::optional<size_t> &scanning_file_min_size,
            const std::vector<std::string> &scanning_masks);
    file_filter file_min_size_filter(size_t file_min_size);
    file_filter file_accepted_masks(const std::vector<std::string>& scanning_masks);
};

#endif // FILESYSTEM_SCANNER_H
