#include <bits/stdc++.h>
#include "error.h"
#include <fcntl.h>
#include "data_structs.h"
using namespace std;

bool registered_atexit = false;
unordered_map<string, int> bindings;
double x_low, x_high, y_low, y_high, z_low, z_high;

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

void update_info(ofstream &info, const internal_data &data, const string &file)
{
    int id_x = file[2] - '0';
    int id_y = file[4] - '0';
    int id_z = file[6] - '0';
    if (id_x == 0) info << x_low << " " << (x_low + x_high) / 2 << " ";
    else info << (x_low + x_high) / 2 << " " << x_high << " ";
    if (id_y == 0) info << y_low << " " << (y_low + y_high) / 2 << " ";
    else info << (y_low + y_high) / 2 << " " << y_high << " ";
    if (id_z == 0) info << z_low << " " << (z_low + z_high) / 2 << "\n";
    else info << (z_low + z_high) / 2 << " " << z_high << "\n";
}

void write_to_file(const string &write_path, const internal_data &data, const string &file)
{
    if (!registered_atexit)
    {
        registered_atexit = true;
        atexit(close_all_files);
    }

    if (bindings.count(file) == 0)
    {
        string full_path = write_path + file + ".info";
        ofstream info(full_path);
        if (!info.is_open())
            critical("There was an error while opening a file: " + full_path);
        update_info(info, data, file);
        info.close();
        full_path = write_path + file + ".raw";
        int fd = open64(full_path.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0666);
        if (fd <= 0)
            critical("There was an error while opening a file: " + to_string(fd));

        bindings[file] = fd;
    }

    write_n(bindings[file], &data, sizeof(internal_data));
}

int coords_to_id(double coord, double low, double high)
{
    double middle = (low + high) / 2;
    if (coord < middle) return 0;
    else return 1;
}


int main(int argc, char* argv[])
{
    //ARGS:
    // - Path name
    // - file name
    string path = argv[1];
    string file_name = argv[2];
    cout << path << " " << file_name << "\n";
    //1. Determine the cords, and find new
    //2. Read file LINE BY LINE and write it to correct file
    string full_path = path + file_name + ".info";
    ifstream info(full_path);
    if (!info.is_open()) {
        critical("No info file");
    }
    info >> x_low >> x_high >> y_low >> y_high >> z_low >> z_high;
    info.close();
    full_path = path + file_name + ".raw";
    int fd = open64(path.c_str(), O_RDONLY, 0666);
    if (fd <= 0) critical("There was an error while opening a file: " + to_string(fd));
    internal_data data;
    int layer = file_name[0] - '0' + 1;
    while (true) {
        int cnt = read(fd, &data, sizeof(internal_data));
        if (cnt != sizeof(internal_data)) critical("Didn't read the whole data!");
        if (cnt <= 0) break;
        int x_id = coords_to_id(data.coords.x, x_low, x_high);
        int y_id = coords_to_id(data.coords.y, y_low, y_high);
        int z_id = coords_to_id(data.coords.z, z_low, z_high);
        write_to_file(path, data, to_string(layer + 1) + "_" + to_string(x_id) + "_" + to_string(y_id) + "_" + to_string(z_id));
    }
}
