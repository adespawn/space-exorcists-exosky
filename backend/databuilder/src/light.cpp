// Liczenie rzeczy związanych z jasnością

#include "light.h"

using namespace std;

light_level parse(const initial_raw_data &data)
{
    return {data.mag_g,
            data.mag_bp,
            data.mean_absolute_v,
            data.ag,
            data.av};
}

double light_intensity(const light_level &light, double distance)
{
    return light.mean_absolute_v + 5 * log10(distance) - 5;
}