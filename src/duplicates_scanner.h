#ifndef DUPLICATES_SCANNER_H
#define DUPLICATES_SCANNER_H

#include "common_aliases.h"

#include <unordered_map>
#include <set>

class duplicates_scanner
{
public:
    using hash_function = std::function<std::size_t(char*, std::size_t)>;

    duplicates_scanner(std::optional<size_t> block_size, std::optional<std::string> hash_algo);
    std::vector<paths> find(const grouped_by_size& files_paths);

private:
    paths analyse_group(const uniq_paths& files_paths);

    template<typename T>
    hash_function hashCrc();

private:
    hash_function _hash;
    size_t _block_size;
};

#endif // DUPLICATES_SCANNER_H
