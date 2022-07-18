#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cassert>
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

    bool operator == (Cube const& other) const {
        return (this->x == other.x && this->y == other.y && this->z == other.z);
    }
};

struct hash_fn {
    size_t operator () (Cube const& cube) const {
        size_t hx = hash<int64_t>()(cube.x);
        size_t hy = hash<int64_t>()(cube.y);
        size_t hz = hash<int64_t>()(cube.z);
        return (hx ^ hy ^ hz);
    }
};

struct ReactorComplex {
    unordered_set<Cube, hash_fn> on_cubes;

    ReactorComplex() = default;
    
    // Reads exactly 1 line in input and sets on/off reactor cubes accordingly
    // DOES NOT CHECK FOR INITIALIZATION REGION
    friend istream& operator >> (istream&, ReactorComplex&); 
    
    // Like >> but checks for initialization region
    // Reads exactly 1 line in ins.
    // Input seems to be well-formatted so that initialization zone is always worked before first full zone procedure...
    // But I'm not receiving it well it's probably fake
    istream& initialize(istream& ins) {
        ins >> ws;
        if (ins.peek() == EOF) { return ins; }

        string operation;
        getline (ins, operation, ' ');
        if (operation != "on" && operation != "off") {
            throw (runtime_error("Invalid istream: operation should be one of on/off."));
        }

        string x_range_bgn;
        string x_range_end;
        getline (ins, x_range_bgn, '.'); // "x=a"
        getline (ins, x_range_end, ','); // ".b"
        int64_t x_begin = stoi(x_range_bgn.substr(2));
        int64_t x_end = stoi(x_range_end.substr(1));
        if (x_begin > 50 || x_end < -50) {
            ins.ignore(__LONG_MAX__, '\n'); // ignore until next line (inclusive of \n)
            // Note that this MAY not work properly in CRLF vs. LF...
            return ins;
        }

        string y_range_bgn;
        string y_range_end;
        getline (ins, y_range_bgn, '.'); // "y=a"
        getline (ins, y_range_end, ','); // ".b"
        int64_t y_begin = stoi(y_range_bgn.substr(2));
        int64_t y_end = stoi(y_range_end.substr(1));
        if (y_begin > 50 || y_end < -50) {
            ins.ignore(__LONG_MAX__, '\n'); // ignore until next line (inclusive of \n)
            // Note that this MAY not work properly in CRLF vs. LF...
            return ins;
        }

        string z_range_bgn;
        string z_range_end;
        getline (ins, z_range_bgn, '.'); // "z=a"
        getline (ins, z_range_end); // ".b"
        int64_t z_begin = stoi(z_range_bgn.substr(2));
        int64_t z_end = stoi(z_range_end.substr(1));
        if (z_begin > 50 || z_end < -50) {
            // ins should already have depleted this line (sans \n character, but >> ws works well)
            return ins;
        }

        // Add into or delete from reactor.on_cubes
        for (int64_t x = x_begin; x <= x_end; x++) {
            if (x < -50) { continue; }
            if (x > 50) { break; }
            for (int64_t y = y_begin; y <= y_end; y++) {
                if (y < -50) { continue; }
                if (y > 50) { break; }
                for (int64_t z = z_begin; z <= z_end; z++) {
                    if (z < -50) { continue; }
                    if (z > 50) { break; }
                    if (operation == "on") { // add
                        on_cubes.insert(Cube(x, y, z));
                    } else { // erase
                        auto it = on_cubes.find(Cube(x, y, z));
                        if (it != on_cubes.end()) { on_cubes.erase(it); }
                    }
                }
            }
        }

        return ins;
    }
};

istream& operator >> (istream& ins, ReactorComplex& reactor) {
    ins >> ws; // remove leading whitespace
    if (ins.peek() == EOF) { return ins; } // Already at EOF => no gain no loss
    // otherwise, read exactly 1 next line that indicates what is to be done
    // (no check, unsafe)

    string operation;
    getline (ins, operation, ' '); // on or off
    if (operation != "on" && operation != "off") {
        throw (runtime_error("Invalid istream: operation should be one of on/off."));
    }

    // x, y, z ranges
    string x_range_bgn;
    string x_range_end;
    getline (ins, x_range_bgn, '.'); // "x=a"
    getline (ins, x_range_end, ','); // ".b"
    int64_t x_begin = stoi(x_range_bgn.substr(2));
    int64_t x_end = stoi(x_range_end.substr(1));

    string y_range_bgn;
    string y_range_end;
    getline (ins, y_range_bgn, '.'); // "y=a"
    getline (ins, y_range_end, ','); // ".b"
    int64_t y_begin = stoi(y_range_bgn.substr(2));
    int64_t y_end = stoi(y_range_end.substr(1));

    string z_range_bgn;
    string z_range_end;
    getline (ins, z_range_bgn, '.'); // "z=a"
    getline (ins, z_range_end); // ".b"
    int64_t z_begin = stoi(z_range_bgn.substr(2));
    int64_t z_end = stoi(z_range_end.substr(1));

    // Add into or delete from reactor.on_cubes
    for (int64_t x = x_begin; x <= x_end; x++) {
        for (int64_t y = y_begin; y <= y_end; y++) {
            for (int64_t z = z_begin; z <= z_end; z++) {
                if (operation == "on") { // add
                    reactor.on_cubes.insert(Cube(x, y, z));
                } else { // erase
                    auto it = reactor.on_cubes.find(Cube(x, y, z));
                    if (it != reactor.on_cubes.end()) { reactor.on_cubes.erase(it); }
                }
            }
        }
    }

    return ins;
}

size_t getSol1(string const& filepath) {
    ifstream inputf (filepath);
    ReactorComplex reactor;
    while (inputf.peek() != EOF) {
        reactor.initialize(inputf);
    }
    return (reactor.on_cubes.size());
}

void test() {
    ReactorComplex test_reactor;
    stringstream test_some ("on x=10..12,y=10..12,z=10..12\noff x=10..12,y=10..12,z=10..12");
    test_some >> test_reactor; // first line read
    assert(test_reactor.on_cubes.size() == 27);
    test_some >> test_reactor; // second line read
    assert(test_reactor.on_cubes.size() == 0);

    stringstream test_bonded ("on x=10..12,y=10..12,z=10..12\non x=-54112..-39298,y=-85059..-49293,z=-27449..7877");
    test_reactor.initialize(test_bonded);
    assert(test_reactor.on_cubes.size() == 27);
    test_reactor.initialize(test_bonded);
    assert(test_reactor.on_cubes.size() == 27);
    stringstream test_half ("on x=49..52,y=-52..-49,z=49..52");
    test_reactor.initialize(test_half);
    assert(test_reactor.on_cubes.size() == 35);

    assert(getSol1("../resource/q21/input_test") == 590784); // Slow!
    // The problem is NOT to simulate the process, but rather 
}

int main() {
    test();
    return 0;
}