#include <iostream>
#include <fstream>
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

    int64_t getSum() const {
        if (type) { // on
            return (upper[0] - lower[0] + 1) * (upper[1] - lower[1] + 1) * (upper[2] - lower[2] + 1);
        } else { // off
            return -((upper[0] - lower[0] + 1) * (upper[1] - lower[1] + 1) * (upper[2] - lower[2] + 1));
        }
    }

    // read exactly one line
    friend istream& operator >> (istream&, Procedure&);

    // convert to input-like form
    friend ostream& operator << (ostream&, Procedure&); 
    
    // Produce complementary procedure from lhs - rhs so that each area is only added (turned on) once in the end
    friend optional<Procedure> intersection (Procedure&, Procedure&); 
};

istream& operator >> (istream& ins, Procedure& init) {

}

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

int64_t getOnReactors (istream& ins, int64_t lower_limit = INT64_MAX, int64_t upper_limit = INT64_MAX) {
    vector<Procedure> procedures;
    while (ins.peek() != EOF) {
        Procedure next;
        ins >> next; // TODO, read 1 line and turn that line into Procedure object
        if (any_of(next.upper.begin(), next.upper.end(), bind2nd(less<int64_t>{}, lower_limit)) || 
            any_of(next.lower.begin(), next.lower.end(), bind2nd(greater<int64_t>{}, lower_limit))) { 
            continue;
        } // TODO: change so that partial intersection within limit is worked properly

        if (next.type) { 
            procedures.push_back(next); 
        } // otherwise, for off procedures we only need to count on cubes affected
        vector<Procedure> buffer;
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
    return (accumulate(procedures.begin(), procedures.end(), static_cast<int64_t>(0), plus<>()));
}










