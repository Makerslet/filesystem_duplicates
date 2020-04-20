#ifndef FILESYSTEM_SCANNER_H
#define FILESYSTEM_SCANNER_H

#include "common_aliases.h"
#include "filters.h"

#include <functional>
#include <queue>
#include <set>

namespace bsys = boost::system;

/**
 * @brief Класс, осуществляющий сканирование файловой системы
 * с целью поиска файлов одинакового размера
 */
class filesystem_scanner
{
public:
    using file_filters = std::vector<file_filter>;
    using dir_filters = std::vector<dir_filter>;

    /**
     * @brief Конструктор
     * @arg scanning_exluded - пути исключенные из сканирования
     * @arg scanning_level - глубина сканирования относительно заданных дирректорий
     * @arg scanning_file_min_size - минимальный размер файла, который подлежит рассмотрению
     * @arg scanning_masks - маски файлов
     */
    filesystem_scanner(const paths &scanning_excluded,
                       std::optional<size_t> scanning_level,
                       std::optional<size_t> scanning_file_min_size,
                       std::vector<std::string> scanning_masks);

    /**
     * @brief Метод сканирования
     * @arg included - пути подлежащие сканированию
     * @return Группы файлов одинакового размера
     */
    grouped_by_size scan(const paths& included);

private:

    /**
     * @brief Метод осуществляющий проверку не находится ли директория для сканирования
     *  глубоко внутри исключенной из сканирования
     * @arg included - пути для сканирования
     * @return Одобренные пути
     */
    paths pre_check(const paths& included);

    /**
     * @brief Метод обработки директории по фильтрам
     * @arg to_scan_dirs - очередь обрабатываемых директорий
     * @arg dir - директория подлежащая обработке
     */
    void handle_dir(std::queue<scan_dir>& to_scan_dirs,
                    const scan_dir dir);

    /**
     * @brief Метод обработки файла по фильтрам
     * @arg result - список сгруппированных по размеру файлов
     * @arg path - путь к файлу, подлежащему обработке
     */
    void handle_file(grouped_by_size& result,
                     const bfs::path& path);

    /**
     * @brief Метод осущесвляющий проход по файловой системе с целью
     *  поиска в заданных директориях фалов одинакового размера
     * @arg included - директории для сканирования
     * @return Список сгруппированных по размеру файлов
     */
    grouped_by_size all_accepted_files(const paths& included);

    /**
     * @brief Метод исключения из контейнера файлов с уникальным размером
     * т.к. они по определению не могут быть дубликатами
     * @arg files - сгруппированные по размеру файлы
     */
    void remove_uniq_sized_files(grouped_by_size& files);

    /**
     * @brief Метод создания фильтров для файлов
     * @arg scanning_file_min_size - минимальный размер файла
     * @arg scanning_masks - маски
     * @return Вектор фильтров
     */
    file_filters create_file_filters(
            const std::optional<size_t>& scanning_file_min_size,
            const std::vector<std::string>& scanning_masks);

    /**
     * @brief Метод создания фильтров для директорий
     * @arg scanning_level - максимальный уровень сканирования
     * @return Вектор фильтров
     */
    dir_filters create_dir_filters(
            const std::optional<size_t>& scanning_level);

private:
    paths _excluded;

    dir_filters _dirs_f;
    file_filters _files_f;
};

#endif // FILESYSTEM_SCANNER_H
