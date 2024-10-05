#ifndef _FILE_OPERATIONS_H
#define _FILE_OPERATIONS_H

#include <bits/stdc++.h>
#include "data_structs.h"

const std::string write_path = "/media/adespawn/01524459-1aff-4407-a4a6-68c6a130a898/data/layer_0";

std::vector<initial_raw_data> read_raw();
void write_to_file(const internal_data &data, const string &file);

#endif
