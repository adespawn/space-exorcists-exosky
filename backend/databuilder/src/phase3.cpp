#include <bits/stdc++.h>
#include "data_structs.h"
#include "error.h"
#include "consts.h"
using namespace std;

bool custom_compare(const internal_data &a, const internal_data &b)
{
    return a.light.mean_absolute_v < b.light.mean_absolute_v;
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

int main() {
    vector<internal_data> res;
    internal_data element;
    while (true)
    {
        int cnt = read(0, &element, sizeof(internal_data));
        if (cnt <= 0)
            break;
        if (cnt != sizeof(internal_data))
            critical("Didn't read the whole data!");

        res.push_back(element);
    }
    sort(res.begin(), res.end(), custom_compare);
    for (int i = 0; i < min(top_size, (int)res.size()); i++)
    {
        write_n(1, &res[i], sizeof(internal_data));
    }    
}