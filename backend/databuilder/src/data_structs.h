#ifndef _DATA_STRUCTS_H
#define _DATA_STRUCTS_H

#include <bits/stdc++.h>

struct __attribute__((packed)) xyz_coordinates
{
    double x;
    double y;
    double z;
};

struct __attribute__((packed)) light_level
{
    float mag_g;
    float mag_bp;
    float mean_absolute_v;
    float ag;
    float av;
};

struct __attribute__((packed)) internal_data
{
    long sun_id;
    xyz_coordinates coords;
    light_level light;

    void log(bool descr = true)
    {
        if (descr)
            std::cout << "sun_id | x | y | z | mag_g | mag_bp | mean_absolute_v | ag | av\n";
        std::cout << sun_id << " " << coords.x << " " << coords.y << " " << coords.z << " " << light.mag_g << " "
                  << light.mag_bp << " " << light.mean_absolute_v << " " << light.ag << " " << light.av << "\n";
    }
};

struct initial_raw_data
{
    long sun_id;

    // Data for distance
    double ra;      // deegrees
    double dec;     // degrees
    float distance; // parsecs

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
        std::cout << sun_id << " " << ra << " " << dec << " " << distance << " " << mag_g << " "
                  << mag_bp << " " << mean_absolute_v << " " << ag << " " << av << "\n";
    }
};

#endif
