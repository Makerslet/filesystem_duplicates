#ifndef COMMON_ALIASES_H
#define COMMON_ALIASES_H

#include <boost/filesystem/path.hpp>
#include <set>
#include <unordered_map>

namespace bfs = boost::filesystem;

using paths = std::vector<bfs::path>;
using uniq_paths = std::set<bfs::path>;
using grouped_by_size = std::unordered_map<size_t, uniq_paths>;

using dir_rel_level = size_t;
using scan_dir = std::pair<bfs::path, dir_rel_level>;

#endif // COMMON_ALIASES_H
