#include "arguments_parser.h"
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

    return 0;
}


