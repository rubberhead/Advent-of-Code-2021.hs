#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <optional>
#include <algorithm>
#include <functional>
#include <numeric>
#include <cassert>
using namespace std;
// Again, peeking at Simon Toth's blog... 
// There really is much to learn from his writings.

const size_t DIMENSIONS = 3;
typedef array<int64_t, DIMENSIONS> Coordinate;

struct Procedure {
    Coordinate lower = {0, 0, 0};
    Coordinate upper = {0, 0, 0};
    bool type; // on - true, off - false

    Procedure() = default;
    Procedure(Coordinate lower, Coordinate upper, bool type) {
        this->lower = lower;
        this->upper = upper;
        this->type = type;
    }

    operator int64_t() const {
        if (type) { // on
            return (upper[0] - lower[0] + 1) * (upper[1] - lower[1] + 1) * (upper[2] - lower[2] + 1);
        } else { // off
            return -((upper[0] - lower[0] + 1) * (upper[1] - lower[1] + 1) * (upper[2] - lower[2] + 1));
        }
    }

    // read exactly one line
    friend istream& operator >> (istream&, Procedure&);

    // convert to input-like form
    // friend ostream& operator << (ostream&, Procedure&); 
    
    // Produce complementary procedure from lhs - rhs so that each area is only added (turned on) once in the end
    friend optional<Procedure> intersection (Procedure&, Procedure&); 
};

istream& operator >> (istream& ins, Procedure& init) {
    // Much like as implemented in day22.cpp
    ins >> ws;
    if (ins.peek() == EOF) { return ins; }

    // op type: on / off corresponds to true / false
    string op;
    getline (ins, op, ' ');
    if (op == "on") {
        init.type = true;
    } else if (op == "off") {
        init.type = false;
    } else {
        throw (runtime_error("Invalid istream: op should be one of on/off."));
    }

    // range
    string x_range_bgn;
    string x_range_end;
    getline (ins, x_range_bgn, '.');
    getline (ins, x_range_end, ',');
    int64_t x_bgn = static_cast<int64_t>(stoi(x_range_bgn.substr(2)));
    int64_t x_end = static_cast<int64_t>(stoi(x_range_end.substr(1)));

    string y_range_bgn;
    string y_range_end;
    getline (ins, y_range_bgn, '.');
    getline (ins, y_range_end, ',');
    int64_t y_bgn = static_cast<int64_t>(stoi(y_range_bgn.substr(2)));
    int64_t y_end = static_cast<int64_t>(stoi(y_range_end.substr(1)));

    string z_range_bgn;
    string z_range_end;
    getline (ins, z_range_bgn, '.');
    getline (ins, z_range_end);
    int64_t z_bgn = static_cast<int64_t>(stoi(z_range_bgn.substr(2)));
    int64_t z_end = static_cast<int64_t>(stoi(z_range_end.substr(1)));

    init.lower = {x_bgn, y_bgn, z_bgn};
    init.upper = {x_end, y_end, z_end};

    // ins.ignore(UINT64_MAX, '\n'); // ignore till next line
    return ins;
}

// ostream& operator >> (...)

optional<Procedure> intersection (Procedure& lhs, Procedure& rhs) {
    int64_t intersection_lower_x = max(lhs.lower[0], rhs.lower[0]);
    int64_t intersection_lower_y = max(lhs.lower[1], rhs.lower[1]);
    int64_t intersection_lower_z = max(lhs.lower[2], rhs.lower[2]);
    int64_t intersection_upper_x = min(lhs.upper[0], rhs.upper[0]);
    int64_t intersection_upper_y = min(lhs.upper[1], rhs.upper[1]);
    int64_t intersection_upper_z = min(lhs.upper[2], rhs.upper[2]);
    if (intersection_lower_x > intersection_upper_x || 
        intersection_lower_y > intersection_upper_y || 
        intersection_lower_z > intersection_upper_z) {
        return nullopt; // if there is no intersection in 1 of 3 axes, there is no interseciton between two procedure
    }

    Coordinate intersection_lower = {intersection_lower_x, intersection_lower_y, intersection_lower_z};
    Coordinate intersection_upper = {intersection_upper_x, intersection_upper_y, intersection_upper_z};
    return (Procedure(intersection_lower, intersection_upper, !lhs.type)); // i.e., procedure that undoes lhs's effect on intersecting area
}

int64_t getOnReactors (istream& ins, int64_t lower_limit = INT64_MIN, int64_t upper_limit = INT64_MAX) {
    vector<Procedure> procedures;
    while (ins.peek() != EOF) {
        Procedure next;
        ins >> next;
        if (any_of(next.upper.begin(), next.upper.end(), bind2nd(less<int64_t>{}, lower_limit)) || 
            any_of(next.lower.begin(), next.lower.end(), bind2nd(greater<int64_t>{}, upper_limit))) { 
            continue;
        } // completely out of range: continue

        // partially out of range: force bound range onto lower/upper limit
        for_each(next.lower.begin(), next.lower.end(), 
            [&](int64_t& entry) {
                if (entry < lower_limit) { entry = lower_limit; }
            }
        );
        for_each(next.upper.begin(), next.upper.end(), 
            [&](int64_t& entry) {
                if (entry > upper_limit) { entry = upper_limit; }
            }
        );
        
        vector<Procedure> buffer;
        if (next.type) { 
            buffer.push_back(next); 
        } // otherwise, for off procedures we only need to count on cubes affected
        for (auto& prev : procedures) {
            auto modifier = intersection(prev, next);
            if (modifier.has_value()) {
                buffer.push_back(modifier.value());
            } else {
                continue;
            }
        }
        for_each(buffer.begin(), buffer.end(), [&](auto& modifier) { procedures.push_back(modifier); });
    }
    int64_t result = 0;
    for_each(procedures.begin(), procedures.end(), 
        [&](auto& p) {
            int64_t psum = p;
            result += psum;
        }
    );
    return (result);
}

void test() {
    // Procedure-reading
    Procedure test_procedure;
    stringstream test_in ("on x=10..12,y=10..12,z=10..12");
    test_in >> test_procedure;
    int64_t test_result = test_procedure;
    assert(test_result == 27);

    // intersection
    // lhs on, rhs on -> double addition, delete intersection once
    Procedure tp1;
    Procedure tp2;
    test_in = stringstream("on x=10..12,y=10..12,z=10..12");
    test_in >> tp1;
    test_in = stringstream("on x=10..12,y=10..12,z=10..12");
    test_in >> tp2;
    auto maybe_procedure = intersection(tp1, tp2);
    assert(maybe_procedure.has_value());
    auto tp12_complement = maybe_procedure.value();
    assert(tp12_complement.type == false);
    assert(tp12_complement.lower == tp1.lower);
    assert(tp12_complement.upper == tp1.upper); 

    // count on cubes from a series of procedures: simple input
    test_in = stringstream("on x=10..12,y=10..12,z=10..12\non x=10..12,y=10..12,z=10..12");
    test_result = getOnReactors(test_in);
    assert(test_result == 27);

    // count on cubes from a series of procedures: input_test
    ifstream inputf ("../resource/q22/input_test");
    test_result = getOnReactors(inputf, -50, 50);
    assert(test_result == 590784);

    // full procedures: input_test_alt
    inputf = ifstream("../resource/q22/input_test_alt");
    test_result = getOnReactors(inputf);
    assert(test_result == 2758514936282235);

}

int main() {
    test();

    // getSol1
    ifstream inputf ("../resource/q22/input");
    int64_t sol1 = getOnReactors(inputf, -50, 50);
    cout << "Part 1: " << sol1 << endl;

    // getSol2
    inputf.seekg(0);
    int64_t sol2 = getOnReactors(inputf);
    cout << "Part 2: " << sol2 << endl;

    return 0;
}









