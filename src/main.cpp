#include "arguments_parser.h"
#include "filesystem_scanner.h"

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
    scan_task task {
                res_value.scanning_paths,
                res_value.scanning_excluded_paths,
                res_value.scanning_level,
                res_value.scanning_file_min_size,
                res_value.scanning_masks};
    filesystem_scanner scanner;
    scanner.scan(task);

    return 0;
}


