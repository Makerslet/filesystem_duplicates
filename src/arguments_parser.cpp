#include "arguments_parser.h"
#include <boost/filesystem/operations.hpp>
#include <iostream>

arguments_parser::arguments_parser() :
    _description("allowed options")
{
    _description.add_options()
            ("h", "show help")

            ("t", bpo::value<
                    std::vector<bfs::path>>(), "set directories for searching")

            ("e", bpo::value<
                    std::vector<bfs::path>>(), "set exluded directories")

            ("l", bpo::value<int>(), "level of scanning, range: [1, ...)")

            ("ms", bpo::value<int>(), "min size file for scanning, range: [1, ...)")

            ("m", bpo::value<
                    std::vector<std::string>>(), "accepted file's masks")

            ("bs", bpo::value<int>(), "block size for scanning, range: [1, 10485760)")

            ("a", bpo::value<std::string>(), "hash algo, range:crc16, crc32");
}

arguments_parser::parse_result arguments_parser::parse(int argc, char **argv)
{
    try{
        auto parsed_options = bpo::parse_command_line(argc, argv, _description);
        bpo::store(parsed_options, _values_storage);

        if (_values_storage.count("h")) {
            std::cout << _description << std::endl;
            return parse_result();
        }

        arguments result;
        // required parameter
        if(_values_storage.count("t"))
        {
            result.scanning_paths = _values_storage["t"].as<std::vector<bfs::path>>();
            for(auto& path : result.scanning_paths)
                if(path.is_relative())
                    path = bfs::canonical(path);
        }
        else
            throw wrong_args_exception("directories for scanning wasn't set");

        // optional parameter
        if(_values_storage.count("e"))
        {
            result.scanning_excluded_paths = _values_storage["e"].as<std::vector<bfs::path>>();
            for(auto& path : result.scanning_excluded_paths)
                if(path.is_relative())
                    path = bfs::canonical(path);
        }

        // optional parameter
        if(_values_storage.count("l"))
        {
            int scan_level = _values_storage["l"].as<int>();
            if(scan_level < 0)
                throw wrong_args_exception("level of scanning can't have negative value");

            result.scanning_level = static_cast<size_t>(scan_level);
        }

        // optional parameter
        if(_values_storage.count("ms"))
        {
            int file_min_size = _values_storage["ms"].as<int>();
            if(file_min_size < 1)
                throw wrong_args_exception("file min size can't have value less than 1 byte");

            result.scanning_file_min_size = static_cast<size_t>(file_min_size);
        }

        // optional parameter
        if(_values_storage.count("m"))
            result.scanning_masks = _values_storage["m"].as<std::vector<std::string>>();

        // optional parameter
        if(_values_storage.count("bs"))
        {
            int block_size = _values_storage["bs"].as<int>();
            if(block_size < 1)
                throw wrong_args_exception("block size can't have value less than 1 byte");
            else if(block_size > 1024 * 1024 * 10)
                throw wrong_args_exception("block size can't have value greater than 10 Mb");

            result.scanning_block_size = static_cast<size_t>(block_size);
        }

        // optional parameter
        if(_values_storage.count("a"))
        {
            std::string hash_algo = _values_storage["a"].as<std::string>();
            if(hash_algo != "crc32" && hash_algo != "crc16")
                throw wrong_args_exception("wrong hashing algorithm");

            result.scanning_hash_algo = hash_algo;
        }

        return result;
    }
    catch(const std::logic_error& ex) {
        std::cout << ex.what() << std::endl;
        std::cout << _description;
        return parse_result();
    }
}
