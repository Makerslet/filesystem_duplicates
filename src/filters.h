#ifndef FILTERS_H
#define FILTERS_H

#include "common_aliases.h"

using file_filter = std::function<bool(const bfs::path&)>;

class file_filter_creator
{
    using masks = std::vector<std::string>;
public:
    static file_filter file_min_size_filter(size_t file_min_size);
    static file_filter file_accepted_masks_filter(const masks& scanning_masks);
};

using dir_filter = std::function<bool(const scan_dir&)>;

class dir_filter_creator
{
public:
    static dir_filter dir_level_filter(size_t level);
    static dir_filter dir_excluded_filter(paths excluded);
};

#endif // FILTERS_H
