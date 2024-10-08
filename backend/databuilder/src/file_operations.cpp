#include "file_operations.h"
#include "error.h"
#include <fcntl.h>

std::string write_path = "/media/adespawn/01524459-1aff-4407-a4a6-68c6a130a898/data/layer_0/";

using namespace std;


// For celestial. 
vector<initial_raw_data> read_raw()
{
    vector<initial_raw_data> res;
    initial_raw_data element;
    while (true)
    {
        int cnt = read(0, &element, sizeof(initial_raw_data));
        if (cnt <= 0)
            break;
        if (cnt != sizeof(initial_raw_data))
            critical("Didn't read the whole data!");

        res.push_back(element);
    }
    return res;
}

int coords_to_id (double val) {
    if (val < -8000) return 0;
    else if (val < -2000) return 1;
    else if (val < 2000) return 2;
    else if (val < 8000) return 3;
    else return 4;
}

void zero_layer_parse_data(const internal_data &data)
{
    int x_id = coords_to_id(data.coords.x);
    int y_id = coords_to_id(data.coords.y);
    int z_id = coords_to_id(data.coords.z);
    // if (x_id == -1 || y_id == -1)
    // {
    //     cout<<"X | Y | X \n";
    //     cout << data.coords.x << " " << data.coords.y << " " << data.coords.z << "\n";
    //     cout << x_id << " " << y_id << " " << z_id << "\n";
    //     warning("Star not in range");
    //     return;
    // }
    string file_name = "0_" + to_string(x_id) + "_" + to_string(y_id) + "_" + to_string(z_id) + ".raw";
    write_to_file(data, file_name);
}

bool registered_atexit = false;
unordered_map<string, int> bindings;

void close_all_files()
{
    for (const auto &[key, value] : bindings)
        close(value);
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

void write_to_file(const internal_data &data, const string &file)
{
    if (!registered_atexit)
    {
        registered_atexit = true;
        atexit(close_all_files);
    }

    if (bindings.count(file) == 0)
    {
        string full_path = write_path + file;
        int fd = open64(full_path.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0666);
        if (fd <= 0)
            critical("There was an error while opening a file: " + to_string(fd));

        bindings[file] = fd;
    }

    write_n(bindings[file], &data, sizeof(internal_data));
}
