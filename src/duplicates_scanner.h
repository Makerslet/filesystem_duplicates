#ifndef DUPLICATES_SCANNER_H
#define DUPLICATES_SCANNER_H

#include <boost/filesystem/path.hpp>
#include <unordered_map>
#include <set>

namespace bfs = boost::filesystem;

class duplicates_scanner
{
public:
    using paths = std::vector<bfs::path>;
    using uniq_paths = std::set<bfs::path>;
    using grouped_by_size = std::unordered_map<size_t, uniq_paths>;

    duplicates_scanner(std::optional<size_t> block_size, std::optional<std::string> hash_algo);
    std::vector<paths> find(const grouped_by_size& files_paths);

private:
    paths analyse_group(const uniq_paths& files_paths);
    size_t hash(char* buffer);

private:
    std::string _hash_algo;
    size_t _block_size;
};

#endif // DUPLICATES_SCANNER_H
