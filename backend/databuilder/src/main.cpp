#include <bits/stdc++.h>
#include "file_operations.h"
#include "data_structs.h"
#include "distances.h"
#include "light.h"
using namespace std;

vector<internal_data> transform(const vector<initial_raw_data> &data)
{
    vector<internal_data> res;
    for (const auto &element : data)
    {
        internal_data new_el;
        new_el.sun_id = element.sun_id;
        new_el.coors = transform(element);
        new_el.light = parse(element);
        res.push_back(new_el);
    }
    return res;
}

int main()
{
    auto raw_data = read_raw();
    auto internal_data = transform(raw_data);
    raw_data[0].log();
    cout << sizeof(internal_data) << " " << sizeof(long) << " " << sizeof(light_level) << " " << sizeof(light_level) << "\n";
}
