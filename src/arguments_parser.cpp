#include "arguments_parser.h"
#include <iostream>

arguments_parser::arguments_parser() :
    _description("allowed options")
{
    _description.add_options()
            ("h", "show help")

            ("t", boost::program_options::value<
                    std::vector<boost::filesystem::path>>(), "set directories for searching")

            ("e", boost::program_options::value<
                    std::vector<boost::filesystem::path>>(), "set exluded directories")

            ("l", boost::program_options::value<size_t>(), "level of scanning")

            ("ms", boost::program_options::value<size_t>(), "min size file for scanning")

            ("m", boost::program_options::value<
                    std::vector<std::string>>(), "accepted file's masks")

            ("bs", boost::program_options::value<size_t>(), "block size for scanning")

            ("a", boost::program_options::value<std::string>(), "hash algo");
}

arguments_parser::parse_result arguments_parser::parse(int argc, char **argv)
{
    auto parsed_options = boost::program_options::parse_command_line(argc, argv, _description);
    boost::program_options::store(parsed_options, _values_storage);

    if (_values_storage.count("h")) {
        std::cout << _description << std::endl;
        return parse_result();
    }

    arguments result;
    // required parameter
    if(_values_storage.count("t"))
        result.scanning_paths = _values_storage["t"].as<std::vector<boost::filesystem::path>>();
    else
        throw wrong_args_exception("directories for scanning wasn't set");

    // optional parameter
    if(_values_storage.count("e"))
        result.scanning_excluded_paths = _values_storage["e"].as<std::vector<boost::filesystem::path>>();

    // optional parameter
    if(_values_storage.count("l"))
        result.scanning_level = _values_storage["l"].as<size_t>();

    // optional parameter
    if(_values_storage.count("ms"))
        result.scanning_file_min_size = _values_storage["ms"].as<size_t>();

    // optional parameter
    if(_values_storage.count("m"))
        result.scanning_masks = _values_storage["m"].as<std::vector<std::string>>();

    // optional parameter
    if(_values_storage.count("bs"))
        result.scanning_block_size = _values_storage["bs"].as<size_t>();

    // optional parameter
    if(_values_storage.count("a"))
        result.scanning_hash_algo = _values_storage["a"].as<std::string>();

    return result;
}
