#ifndef DUPLICATES_SCANNER_H
#define DUPLICATES_SCANNER_H

#include "common_aliases.h"

#include <unordered_map>
#include <set>

/**
 * @brief Класс, осуществляющий сравнение файлов одинакового размера
 */
class duplicates_scanner
{
public:
    using hash_function = std::function<std::size_t(char*, std::size_t)>;

    /**
     * @brief Конструктор
     * @arg block_size - размер блока для чтения
     * @arg hash_algo - название алгоритма хеширования
     */
    duplicates_scanner(std::optional<size_t> block_size, std::optional<std::string> hash_algo);

    /**
     * @brief Метод поиска дубликатов среди групп файлов одинакового размера
     * @arg files_paths - пути к файлам, сгруппированные по размеру
     * @return Сгруппированные дубликаты
     */
    std::vector<paths> find(const grouped_by_size& files_paths);

private:
    /**
     * @brief Метод поиска дубликатов среди одной группы файлов одинакового размера
     * @arg files_paths - пути к файлам одного размера
     * @return Вектор дубликатов
     */
    paths analyse_group(const uniq_paths& files_paths);

    /**
     * @brief Метод генерации функтора с хеш функцией
     * @return Функтор применяющий внутри себя заданную шаблоном хеш функцию
     */
    template<typename T>
    hash_function hash_creator();

private:
    hash_function _hash;
    size_t _block_size;
};

#endif // DUPLICATES_SCANNER_H
