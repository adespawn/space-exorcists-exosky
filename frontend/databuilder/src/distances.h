#ifndef _DISTANCES_H
#define _DISTANCES_H

#include <bits/stdc++.h>
#include "data_structs.h"

struct xyz_coordinates
{
    double x;
    double y;
    double z;
};


xyz_coordinates transform(const initial_raw_data &data);

#endif
