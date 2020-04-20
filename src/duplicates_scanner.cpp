#include "duplicates_scanner.h"

#include <boost/crc.hpp>

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
    {
        std::string hash_str = hash_algo.value();
        if(hash_str == "crc16")
            _hash = hash_creator<boost::crc_16_type>();
        else if(hash_str == "crc32")
            _hash = hash_creator<boost::crc_32_type>();
        else
            throw std::runtime_error("unknown hashing algorithm");
    }
    else
        _hash = hash_creator<boost::crc_32_type>();
}

std::vector<paths> duplicates_scanner::find(const grouped_by_size& files)
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

template<typename T>
duplicates_scanner::hash_function duplicates_scanner::hashCrc()
{
    return [](char* data, std::size_t size) {
        T hash_algo;
        hash_algo.process_bytes(data, size);
        return hash_algo.checksum();
    };
}

paths duplicates_scanner::analyse_group(const uniq_paths& files_paths)
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
            value.second = _hash(buffer.data(), buffer.size());
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
