#ifndef ARGUMENTS_PARSER_H
#define ARGUMENTS_PARSER_H

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <boost/filesystem/path.hpp>

struct arguments {
    std::vector<boost::filesystem::path> scanning_paths;
    std::vector<boost::filesystem::path> scanning_excluded_paths;
    std::optional<size_t> scanning_level;
    std::optional<size_t> scanning_file_min_size;
    std::vector<std::string> scanning_masks;
    std::optional<size_t> scanning_block_size;
    std::optional<std::string> scanning_hash_algo;
};


class arguments_parser
{
public:
    class wrong_args_exception : public std::invalid_argument
    {
    public:
        wrong_args_exception(const std::string& err) :
            std::invalid_argument(err) {}
    };
    using parse_result = std::optional<arguments>;

    arguments_parser();
    parse_result parse(int argc, char** argv);

private:
    boost::program_options::options_description _description;
    boost::program_options::variables_map _values_storage;
};

#endif // ARGUMENTS_PARSER_H
