#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#include <cassert>
#include <numeric>
#include <algorithm>
using namespace std;

class xyPair {
    public: 
        int x;
        int y;

        xyPair (int _x, int _y) {
            x = _x;
            y = _y;
        }

        xyPair operator + (xyPair const &other_pair) {
            xyPair sum (x + other_pair.x, y + other_pair.y);
            return sum;
        }

        xyPair operator - (xyPair const &dest_pair) {
            xyPair diff (x - dest_pair.x, y - dest_pair.y);
            return diff;
        }

        xyPair operator * (double const &constant) {
            xyPair mult (x * constant, y * constant);
            return mult;
        }

        bool operator == (xyPair const &other_pair) {
            return (x == other_pair.x && y == other_pair.y);
        }

        int cross (xyPair w) {
            return (x * w.y - y * w.x);
        }

        int dot (xyPair w) {
            return (x * w.x + y * w.y);
        }

};

// slapstick fix for no match to 'operator<'
bool operator < (xyPair const &lhs, xyPair const &rhs) {
    if (lhs.x == rhs.x) {
        return (lhs.y < rhs.y);
    } 
    return (lhs.x < rhs.x);
}

// Returns whether b is in line segment a-c.
// Assumes a, b, c collinear.
bool isInBetween (xyPair a, xyPair b, xyPair c) {
    int dot_ba_ca = (b - a).dot(c - a);
    int sq_length_ca = (c - a).dot(c - a);
    if (dot_ba_ca < 0 || dot_ba_ca > sq_length_ca) {
        return false;
    }
    return true;
}

typedef struct vectorLine {
    xyPair start; 
    xyPair end;
};

/*
    Check whether two vectorLines intersect.
    
    Supposed two vectorLines run from <p> -> <p> + <r> and <q> -> <q> + <s>. <r> and <s> can be computed from 
    (<p> + <r>) - <p> and likewise for <q>. 

    Denote vectorLine l1 = <p> + t<r> and l2 = <q> + u<s>. t and u range from [0, 1].
    In this way direction vector is normalized and any potential intersection MUST have t' and u' btwn [0, 1]. 

    Returns the pointer to vector of intersecting point(s) as provided (In non-intersecting cases no elements would 
    be pushed into the vector).
*/
vector<xyPair> * intersections(vectorLine l1, vectorLine l2, vector<xyPair> &result) {
    // Direction vectors
    xyPair r = l1.end - l1.start;
    xyPair s = l2.end - l2.start;

    // Segment point vectors
    xyPair p = l1.start;
    xyPair q = l2.start;
    
    // 1. Check parallel (i.e., whether r × s == 0)
    if (r.cross(s) != 0) { // i.e., not parallel
        // Two cases: 
        // Either two segments also cross (at one point)
        // or two segments does not reach each other (at no point)
        double t = ((double) (q - p).cross(s)) / (r.cross(s));
        double u = ((double) (q - p).cross(r)) / (r.cross(s));
        if (0 <= t && t <= 1 && 0 <= u && u <= 1) {
            xyPair intersection = p + (r * t);
            result.push_back(intersection);
            return (&result);
        }
        return (&result);
    } else { // i.e., parallel
        // Three cases:
        // 1. Two segments are collinear but not intersecting
        // 2. Two segments are collinear and intersecting (at multiple points)
        // 3. Two segments are strictly parallel (hence not intersecting)
        if ((q - p).cross(r) == 0) { // i.e., if two segments are collinear
            // iterate through each point on l2 and check whether it is on l1.
            int gcd_xy = gcd(s.x, s.y);
            xyPair normalized_s (s.x / gcd_xy, s.y / gcd_xy);

            // Approaches l2.end, so less / greater than cannot suffice
            // Check if each discrete point on l2 is also on l1
            q = q - normalized_s;
            while (!(q.x == l2.end.x && q.y == l2.end.y)) {
                q = q + normalized_s;
                if (isInBetween(l1.start, q, l1.end)) { 
                    xyPair intersection (q.x, q.y);
                    result.push_back(intersection);
                }
            }

            return (&result);
        } else { // otherwise parallel
            return (&result);
        }
    }
}

vector<vectorLine> parser(string filepath, bool vertical_horizontal_only = false) {
    ifstream inputf (filepath);
    string nextline;
    
    vector<vectorLine> lines;
    while (inputf.peek() != EOF) {
        getline (inputf, nextline);
        // Parse first and second xyPairs
        // Assuming each line is structured as x,y -> a,b
        char* token_1 = strtok(&(nextline[0]), ",");
        char* token_2 = strtok(nullptr, " ");
        xyPair startPair (stoi(token_1), stoi(token_2));

        strtok(nullptr, " "); // Pattern-matching
        char* token_3 = strtok(nullptr, ",");
        char* token_4 = strtok(nullptr, ","); // last char so whatever limiter helps really
        xyPair endPair (stoi(token_3), stoi(token_4));

        if (vertical_horizontal_only) {
            if ((startPair.x == endPair.x) || (startPair.y == endPair.y)) {
                lines.push_back({startPair, endPair});
                //cout << "(" << token_1 << "," << token_2 << ") -> ";
                //cout << "(" << token_3 << "," << token_4 << ")" << endl;
            }
            continue;
        }

        lines.push_back({startPair, endPair});
    }

    return (lines);
} 



// Get size of overlapping point set resulting from each line insertion.
map<xyPair, int> * getSol1(vector<vectorLine> &lines, map<xyPair, int> &overlaps) {
    for (int i = 0; i < lines.size(); i++) {
        for (int j = i + 1; j < lines.size(); j++) {
            vector<xyPair> intersectionSet;
            intersections(lines[i], lines[j], intersectionSet);
            if (intersectionSet.size() > 0) {
                for (xyPair point : intersectionSet) {
                    try {
                        overlaps.at(point) ++;
                    } catch (out_of_range) {
                        overlaps[point] = 2;
                    }
                }
            }
        }
    }

    return (&overlaps);
}

void test() {
    vectorLine l1 = {xyPair (0, 9), xyPair (5, 9)};
    vectorLine l2 = {xyPair (8, 0), xyPair (0, 8)};
    vectorLine l3 = {xyPair (9, 4), xyPair (3, 4)};
    vectorLine l4 = {xyPair (2, 2), xyPair (2, 1)};
    vectorLine l5 = {xyPair (7, 0), xyPair (7, 4)};
    vectorLine l6 = {xyPair (6, 4), xyPair (2, 0)};
    vectorLine l7 = {xyPair (0, 9), xyPair (2, 9)};
    vectorLine l8 = {xyPair (3, 4), xyPair (1, 4)};
    vectorLine l9 = {xyPair (0, 0), xyPair (8, 8)};
    vectorLine l10 = {xyPair (5, 5), xyPair (8, 2)};

    // Intersection Check
    vector<xyPair> intersectionSet;
    vector<xyPair> * results = intersections(l5, l9, intersectionSet);
    assert(results->size() == 0);
    results = intersections(l1, l7, intersectionSet);
    assert(&intersectionSet == results);
    assert(intersectionSet.size() == 3);
    intersectionSet.clear();
    results = intersections(l9, l10, intersectionSet);
    assert(intersectionSet.size() != 0);

    // Parser
    vector<vectorLine> parsedLines = parser("../resource/q5/input_test");
    assert(parsedLines[0].start == l1.start && parsedLines[0].end == l1.end);
    assert(parsedLines[9].start == l10.start && parsedLines[9].end == l10.end);

    // getSol1
    parsedLines = parser("../resource/q5/input_test", true);
    map<xyPair, int> sol1;
    getSol1(parsedLines, sol1);
    /*
    for (auto it = sol1.begin(); it != sol1.end(); it++) {
        cout << "(" << it->first.x << "," << it->first.y << "): " << it->second << endl;
    }*/
    assert(sol1.size() == 5);
    parsedLines = parser("../resource/q5/input_test");
    sol1.clear();
    getSol1(parsedLines, sol1);
    /*
    for (auto it = sol1.begin(); it != sol1.end(); it++) {
        cout << "(" << it->first.x << "," << it->first.y << "): " << it->second << endl;
    }*/
    assert(sol1.size() == 12);
}

int main() {
    test();
    vector<vectorLine> parsedLines = parser("../resource/q5/input", true);
    map<xyPair, int> intersectingPoints;
    getSol1(parsedLines, intersectingPoints);
    cout << intersectingPoints.size() << endl;
    
    return 0;
}

