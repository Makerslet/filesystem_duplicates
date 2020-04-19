#include "duplicates_scanner.h"

#include <fstream>
#include <unordered_map>

duplicates_scanner::duplicates_scanner(
        std::optional<size_t> block_size,
        std::optional<std::string> hash_algo)
{
    if(block_size.has_value())
        _block_size = block_size.value();
    else
        _block_size = 4 * 1024;

    if(hash_algo.has_value())
        _hash_algo = hash_algo.value();
    else
        _hash_algo = "md5";
}

std::vector<duplicates_scanner::paths> duplicates_scanner::find(const grouped_by_size& files)
{
    std::vector<paths> result;

    for(const auto group : files)
    {
        auto summary = analyse_group(group.second);
        if(!summary.empty())
            result.push_back(summary);
    }

    return result;
}

size_t duplicates_scanner::hash(char* buffer)
{
    size_t result = 0;

    for(size_t i = 0; i < _block_size; ++i)
        result += buffer[i];

    return result;
}

duplicates_scanner::paths duplicates_scanner::analyse_group(const uniq_paths& files_paths)
{
    paths result;
    std::unordered_map<std::string, std::pair<std::fstream, size_t>> hashes;

    for(const bfs::path& p : files_paths)
    {
        std::string path_str = p.string();
        std::fstream read_stream(path_str, std::fstream::in);
        hashes[path_str] = std::make_pair(std::move(read_stream), 0);
    }

    std::vector<char> buffer(_block_size);

    bool end_of_files = false;
    while(!hashes.empty() && !end_of_files)
    {
        for(auto& elem : hashes)
        {
            memset(buffer.data(), 0, _block_size);
            auto& value = elem.second;

            auto reading_result = value.first.readsome(buffer.data(), _block_size);
            end_of_files = static_cast<size_t>(reading_result) < _block_size;
            value.second = hash(buffer.data());
        }

        auto iter = hashes.begin();
        while(iter != hashes.end())
        {
            size_t num_elems = 0;
            size_t current_hash = iter->second.second;

            std::for_each(hashes.begin(), hashes.end(), [current_hash, &num_elems](const auto& value)
            {if(value.second.second == current_hash) ++num_elems;});

            if(num_elems == 1)
                iter = hashes.erase(iter);
            else
                ++iter;
        }
    }

    for(auto& elem : hashes)
        result.push_back(elem.first);

    return result;
}
