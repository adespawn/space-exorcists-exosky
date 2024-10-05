#ifndef _LIGHT_H
#define _LIGHT_H

#include <bits/stdc++.h>
#include "data_structs.h"

light_level parse(const initial_raw_data &data);
double light_intensity(const light_level &light, double distance);

#endif
