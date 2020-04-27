#ifndef ARGUMENTS_PARSER_H
#define ARGUMENTS_PARSER_H

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <boost/filesystem/path.hpp>

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;

/**
 * @brief Структура содержащая результаты парсинга входных аргументов
 */
struct arguments {
    /**
     * @details Пути для сканирования
     */
    std::vector<bfs::path> scanning_paths;
    /**
     * @details Пути исключаемы из сканирования
     */
    std::vector<bfs::path> scanning_excluded_paths;
    /**
     * @details Глубина сканирования
     */
    std::optional<size_t> scanning_level;
    /**
     * @details Минимальный размер файла, подлежащий рассмотрению
     */
    std::optional<size_t> scanning_file_min_size;
    /**
     * @details Маски файлов
     */
    std::vector<std::string> scanning_masks;
    /**
     * @details Размер блока при чтении файла
     */
    std::optional<size_t> scanning_block_size;
    /**
     * @details Алгоритм хеширования
     */
    std::optional<std::string> scanning_hash_algo;
};


/**
 * @brief Класс парсинга аргументов командной строки
 *  Опирается на boost program options
 */
class arguments_parser
{
public:

    /**
     * @brief Класс исключения ошибочных аргументов командной строки
     */
    class wrong_args_exception : public std::logic_error
    {
    public:
        wrong_args_exception(const std::string& err) :
            std::logic_error(err) {}
    };
    using parse_result = std::optional<arguments>;


    /**
     * @brief Конструктор
     */
    arguments_parser();

    /**
     * @brief Метод парсинга аргументов командной строки
     * @arg argc - количество аргументов
     * @arg argv - массив аргументов
     * @return Результат парсинга
     */
    parse_result parse(int argc, char** argv);

private:
    bpo::options_description _description;
    bpo::variables_map _values_storage;
};

#endif // ARGUMENTS_PARSER_H
