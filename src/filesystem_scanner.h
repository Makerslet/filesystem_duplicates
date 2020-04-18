#ifndef FILESYSTEM_SCANNER_H
#define FILESYSTEM_SCANNER_H

#include <boost/filesystem/path.hpp>
#include <functional>
#include <queue>
#include <set>

namespace bfs = boost::filesystem;
namespace bsys = boost::system;

struct scan_task {
    std::vector<bfs::path> scanning_paths;
    std::vector<bfs::path> scanning_excluded_paths;
    std::optional<size_t> scanning_level;
    std::optional<size_t> scanning_file_min_size;
    std::vector<std::string> scanning_masks;
};

class filesystem_scanner
{
public:
    using paths = std::vector<bfs::path>;
    using file_filter = std::function<bool(const bfs::path&)>;
    using file_filters = std::vector<file_filter>;

    using uniq_paths = std::set<bfs::path>;
    using grouped_by_size = std::unordered_map<size_t, uniq_paths>;

    using dir_rel_level = size_t;
    using scan_dir = std::pair<bfs::path, dir_rel_level>;
    using dir_filter = std::function<bool(const scan_dir&)>;
    using dir_filters = std::vector<dir_filter>;

    filesystem_scanner();
    grouped_by_size scan(const scan_task& task);

private:
    void handle_dir(std::queue<scan_dir>& to_scan_dirs,
                    const dir_filters &dir_f,
                    const scan_dir dir);

    void handle_file(grouped_by_size& result,
                     const file_filters& file_f,
                     const bfs::path& path);

    grouped_by_size all_accepted_files(
            const paths& included,
            const dir_filters &dir_f,
            const file_filters& file_f);

    void remove_uniq_sized_files(grouped_by_size& files);

    file_filters create_file_filters(
            const std::optional<size_t> &scanning_file_min_size,
            const std::vector<std::string> &scanning_masks);
    file_filter file_min_size_filter(size_t file_min_size);
    file_filter file_accepted_masks(const std::vector<std::string>& scanning_masks);

    dir_filters create_dir_filters(
            const std::optional<size_t>& scanning_level,
            const paths& excluded);
    dir_filter dir_level_filter(size_t level);
    dir_filter dir_excluded_filter(paths excluded);
};

#endif // FILESYSTEM_SCANNER_H
