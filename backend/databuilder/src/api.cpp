#include <bits/stdc++.h>
#include "data_structs.h"
#include "error.h"
#include "consts.h"
#include "light.h"
#include "distances.h"
using namespace std;

bool custom_compare(const final_data &a, const final_data &b)
{
    return a.light_intensity < b.light_intensity;
}

void write_n(int fd, const void *vptr, size_t n)
{
    // cout<<"Trying to write to fd: "<<fd<<" data of size: ";
    ssize_t nleft, nwritten;
    const char *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
            critical("There was a serious error while writing a file: " + to_string(nwritten) + ", errno: " + to_string(errno));
        nleft -= nwritten;
        ptr += nwritten;
    }
    return;
}

int main(int argc, char* argv[]) {
    double x = stod(argv[1]);
    double y = stod(argv[2]);
    double z = stod(argv[3]);
    map <long, bool> already_seen;
    xyz_coordinates coords = {x, y, z};
    vector<final_data> res;
    internal_data element;
    while (true)
    {
        int cnt = read(0, &element, sizeof(internal_data));
        if (cnt <= 0)
            break;
        if (cnt != sizeof(internal_data))
            critical("Didn't read the whole data!");
        if (already_seen.count(element.sun_id) > 0) continue;
        already_seen[element.sun_id] = true;
        res.push_back({element.sun_id, element.coords, light_intensity(element.light.mean_absolute_v, calculate_distance(element.coords, coords))});
    }
    sort(res.begin(), res.end(), custom_compare);
    for (int i = 0; i < min(top_size, (int)res.size()); i++)
    {
        cout << res[i].sun_id << " " << res[i].coords.x << " " << res[i].coords.y << " " << res[i].coords.z << " " << res[i].light_intensity << "\n";
    }
}