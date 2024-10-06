#include <bits/stdc++.h>

using namespace std;

constexpr int nor = 5;
struct datamodel
{
    long id;

    // float data[nor];
    double d1;
    double d2;
    float d3;

    float color[nor];
    // std::float16_t

    void read(long id_)
    {
        id = id_;
        cin >> d1 >> d2 >> d3;
        for (int i = 0; i < nor; i++)
            cin >> color[i];
        
    }
};

int main()
{
    ios_base::sync_with_stdio(0);
    string headers;
    // cin >> headers;
    string line;
    sizeof(datamodel);
    int cnt = 0;
    long start;
    while (cin >> start)
    {
        // cnt++;
        datamodel model;
        model.read(start);
        // cout << sizeof(model);
        char *ptr = (char *)&model;
        for (int i = 0; i < sizeof(model); i++)
            putchar(ptr[i]);
        // exit(0);
    }
}
