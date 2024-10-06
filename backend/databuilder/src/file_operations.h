#ifndef _FILE_OPERATIONS_H
#define _FILE_OPERATIONS_H

#include <bits/stdc++.h>
#include "data_structs.h"

std::vector<initial_raw_data> read_raw();
void write_to_file(const internal_data &data, const std::string &file);
void zero_layer_parse_data(const internal_data &data);

#endif
