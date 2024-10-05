#ifndef _DATA_STRUCTS_H
#define _DATA_STRUCTS_H

#include <bits/stdc++.h>

struct initial_raw_data
{
    long sun_id;

    // Data for distance
    double ra; //deegrees
    double dec; //degrees
    float distance; //parsecs

    // Data for brightness
    float mag_g;
    float mag_bp;
    float mean_absolute_v;
    float ag;
    float av;

    void log(bool descr = true)
    {
        if (descr)
            std::cout << "sun_id | ra | dec | distance | mag_g | mag_bp | mean_absolute_v | ag | av\n";
        std::cout << sun_id << " " << ra << " " << dec << " " << distance << " "
             << mag_bp << " " << mean_absolute_v << " " << ag << " " << av << "\n";
    }
};

#endif
