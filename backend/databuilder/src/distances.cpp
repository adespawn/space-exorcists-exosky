// przekształcanie współrzędnych
// liczenie odległości

#include "distances.h"

using namespace std;

xyz_coordinates transform(const initial_raw_data &data)
{
    long double ra = (long double)data.ra * (long double)M_PI / (long double)180;
    long double dec = (long double)data.dec * (long double)M_PI / (long double)180;
    long double distance = (long double)data.distance;

    double x = distance * (long double)cos(dec) * (long double)cos(ra);
    double y = distance * (long double)cos(dec) * (long double)sin(ra);
    double z = distance * (long double)sin(dec);

    return {x, y, z};
}

double calculate_distance(const xyz_coordinates &a, const xyz_coordinates &b)
{
    return sqrt(pow((long double)a.x - (long double)b.x, 2) + pow((long double)a.y - (long double)b.y, 2) + pow((long double)a.z - (long double)b.z, 2));
}
