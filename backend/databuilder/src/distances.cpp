// przekształcanie współrzędnych
// liczenie odległości

#include "distances.h"

using namespace std;

xyz_coordinates transform(const initial_raw_data &data) {
    double ra = data.ra * M_PI / 180;
    double dec = data.dec * M_PI / 180;
    double distance = data.distance;

    double x = distance * cos(dec) * cos(ra);
    double y = distance * cos(dec) * sin(ra);
    double z = distance * sin(dec);

    return {x, y, z};
}
