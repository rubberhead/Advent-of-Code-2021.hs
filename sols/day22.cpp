#include <iostream>
#include <fstream>
#include <unordered_set>
using namespace std;

struct Cube {
    int64_t x;
    int64_t y;
    int64_t z;

    Cube() = delete;
    Cube(Cube&&) = default;
    Cube(int64_t x, int64_t y, int64_t z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    bool operator == (Cube const& other) {
        return (this->x == other.x && this->y == other.y && this->z == other.z);
    }
};

auto hash_fn = [](Cube const& cube) {
    size_t hx = hash<int64_t>()(cube.x);
    size_t hy = hash<int64_t>()(cube.y);
    size_t hz = hash<int64_t>()(cube.z);
    return (hx ^ hy ^ hz);
};

struct ReactorComplex {
    unordered_set<Cube, decltype(hash_fn)> on_cubes;

    ReactorComplex() = default;
    
    // Reads exactly 1 line in input and sets on/off reactor cubes accordingly
    friend istream& operator >> (istream&, ReactorComplex&); 
};

istream& operator >> (istream& ins, ReactorComplex& reactor) {
    string nextline;
    if (ins.peek() == EOF) { return ins; } // Already at EOF => no gain no loss
    getline (ins, nextline);

    string operation;
    getline (ins, operation, ' '); // on or off
    if (operation != "on" || operation != "off") {
        throw (runtime_error("Invalid istream: operation should be one of on/off."));
    }

    // x, y, z ranges
    string x_range_bgn;
    string x_range_end;
    getline (ins, x_range_bgn, '.'); // " x=a"
    getline (ins, x_range_end, ','); // "..b"
    int64_t x_begin = stoi(x_range_bgn.substr(3));
    int64_t x_end = stoi(x_range_end.substr(2));

    string y_range_bgn;
    string y_range_end;
    getline (ins, y_range_bgn, '.'); // ",y=a"
    getline (ins, y_range_end, ','); // "..b"
    int64_t y_begin = stoi(y_range_bgn.substr(3));
    int64_t y_end = stoi(y_range_end.substr(2));

    string z_range_bgn;
    string z_range_end;
    getline (ins, z_range_bgn, '.'); // ",z=a"
    getline (ins, z_range_end); // "..b"
    int64_t z_begin = stoi(z_range_bgn.substr(3));
    int64_t z_end = stoi(z_range_end.substr(2));

    // Add into or delete from reactor.on_cubes
    for (int64_t x = x_begin; x <= x_end; x++) {
        for (int64_t y = y_begin; y <= y_end; y++) {
            for (int64_t z = z_begin; z <= z_end; z++) {
                if (operation == "on") { // add
                    
                } else { // erase

                }
            }
        }
    }
}
