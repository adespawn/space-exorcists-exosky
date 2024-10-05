#ifndef _DISTANCES_H
#define _DISTANCES_H

#include <bits/stdc++.h>
#include "data_structs.h"

xyz_coordinates transform(const initial_raw_data &data);
double calculate_distance(const xyz_coordinates &a, const xyz_coordinates &b);

#endif
