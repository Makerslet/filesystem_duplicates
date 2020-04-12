#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

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
    boost::program_options::options_description description("Allowed options");
    description.add_options()
            ("help", "Type exit for exit")
            ("length", boost::program_options::value<uint>(), "set bulk length");

    boost::program_options::variables_map values_storage;
    auto parsed_options = boost::program_options::parse_command_line(argc, argv, description);
    boost::program_options::store(parsed_options, values_storage);

    if (values_storage.count("help")) {
        std::cout << description << std::endl;
        return 0;
    }

    uint bulk_length = 0;
    if(!values_storage.count("length"))
    {
        std::cout << "bulk length wasn't set" << std::endl;
        return 1;
    }
    else
    {
        std::size_t tmp = values_storage["length"].as<uint>();
        if(tmp > 0)
            bulk_length = tmp;
        else {
            std::cout << "bulk length can't be 0" << std::endl;
            return 1;
        }
    }

    std::string argument;
    commands_factory cmd_factory;
    command_type cmd_type;
    command_handler cmd_handler(bulk_length);

    auto console_out = std::make_shared<console_printer>();
    auto file_out = std::make_shared<file_printer>();

    cmd_handler.subscribe(console_out);
    cmd_handler.subscribe(file_out);

    do {
        std::getline(std::cin, argument);
        auto command = cmd_factory.create_command(argument);
        cmd_type = command->type();
        cmd_handler.add_command(std::move(command));

    } while(cmd_type != command_type::finish);

    return 0;
}


