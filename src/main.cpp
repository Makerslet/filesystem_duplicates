#include "arguments_parser.h"
#include "filesystem_scanner.h"
#include "duplicates_scanner.h"

#include <iostream>

/**
 * @brief Entry point
 *
 * Execution of the program
 * starts here.
 *
 * @return Program exit status
 */

int main (int argc, char** argv)
{
    arguments_parser args_parser;
    auto result = args_parser.parse(argc, argv);
    if(!result.has_value())
        return 0;

    auto res_value = result.value();
    filesystem_scanner scanner(
                res_value.scanning_excluded_paths,
                res_value.scanning_level,
                res_value.scanning_file_min_size,
                res_value.scanning_masks);
    auto files_to_check = scanner.scan(res_value.scanning_paths, res_value.scanning_excluded_paths);

    duplicates_scanner files_scanner(res_value.scanning_block_size, res_value.scanning_hash_algo);
    for(const auto& group : files_scanner.find(files_to_check))
    {
        for(const auto& file : group)
            std::cout << file << std::endl;

        std::cout << std::endl;
    }

    return 0;
}


