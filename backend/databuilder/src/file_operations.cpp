#include "file_operations.h"
#include "error.h"

using namespace std;

vector<initial_raw_data> read_raw()
{
    vector<initial_raw_data> res;
    initial_raw_data element;
    while (true)
    {
        int cnt = read(0, &element, sizeof(initial_raw_data));
        if (cnt <= 0)
            break;
        if(cnt != sizeof(initial_raw_data))
            critical("Didn't read the whole data!");
        
        res.push_back(element);
    }
    return res;
}
