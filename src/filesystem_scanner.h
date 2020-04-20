#ifndef FILESYSTEM_SCANNER_H
#define FILESYSTEM_SCANNER_H

#include "common_aliases.h"
#include "filters.h"

#include <functional>
#include <queue>
#include <set>

namespace bsys = boost::system;

class filesystem_scanner
{
public:
    using file_filters = std::vector<file_filter>;
    using dir_filters = std::vector<dir_filter>;

    filesystem_scanner(const paths &scanning_exluded,
                       std::optional<size_t> scanning_level,
                       std::optional<size_t> scanning_file_min_size,
                       std::vector<std::string> scanning_masks);

    grouped_by_size scan(const paths& included, const paths& excluded);

private:
    paths pre_check(const paths& included, const paths& exluded);
    void handle_dir(std::queue<scan_dir>& to_scan_dirs,
                    const scan_dir dir);

    void handle_file(grouped_by_size& result,
                     const bfs::path& path);

    grouped_by_size all_accepted_files(const paths& included);

    void remove_uniq_sized_files(grouped_by_size& files);

    file_filters create_file_filters(
            const std::optional<size_t> &scanning_file_min_size,
            const std::vector<std::string> &scanning_masks);

    dir_filters create_dir_filters(
            const std::optional<size_t>& scanning_level,
            const paths& excluded);

private:
    dir_filters _dirs_f;
    file_filters _files_f;
};

#endif // FILESYSTEM_SCANNER_H
