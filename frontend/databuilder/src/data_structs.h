#ifndef _DATA_STRUCTS_H
#define _DATA_STRUCTS_H

#include <bits/stdc++.h>

struct initial_raw_data
{
    long sun_id;

    // Data for distance
    double ra;
    double dec;
    float distance;

    // Data for brightness
    float mag_g;
    float mag_bp;
    float mean_absolute_v;
    float ag;
    float av;
};

#endif
