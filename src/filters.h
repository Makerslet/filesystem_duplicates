#ifndef FILTERS_H
#define FILTERS_H

#include "common_aliases.h"

using file_filter = std::function<bool(const bfs::path&)>;

/**
 * @brief Класс для создания фильтров файлов
 */
class file_filter_creator
{
    using masks = std::vector<std::string>;
public:
    /**
     * @brief Метод, генерирующий функтор - фильтр по минимальному
     * размеру файла
     * @arg file_min_size - минимальный размер файла
     * @return Функтор - фильтр проверяющий соответствие размера файла
     */
    static file_filter file_min_size_filter(size_t file_min_size);

    /**
     * @brief Метод, генерирующий функтор - фильтр по маскам
     * @arg scanning_masks - маски сканирования
     * @return Функтор - фильтр проверяющий соответствие маскам
     */
    static file_filter file_accepted_masks_filter(const masks& scanning_masks);
};

using dir_filter = std::function<bool(const scan_dir&)>;

/**
 * @brief Класс для создания фильтров директорий
 */
class dir_filter_creator
{
public:

    /**
     * @brief Метод, генерирующий функтор - фильтр по уровню сканирования
     * @arg level - ограничительный уровень сканирования
     * @return Функтор - фильтр проверяющий соответствие уровню сканирования
     */
    static dir_filter dir_level_filter(size_t level);


    /**
     * @brief Метод, генерирующий функтор - фильтр по исключаемым путям
     * @arg paths - исключенные пути
     * @return Функтор - фильтр проверяющий по исключенным путям
     */
    static dir_filter dir_excluded_filter(paths excluded);
};

#endif // FILTERS_H
