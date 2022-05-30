#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <numeric>
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

    Returns either the pointer to vector of intersecting point(s) in discrete form or nullptr if none exists.
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
        return (nullptr);
    } else { // i.e., parallel
        // Three cases:
        // 1. Two segments are collinear but not intersecting
        // 2. Two segments are collinear and intersecting (at multiple points)
        // 3. Two segments are strictly parallel (hence not intersecting)
        if ((q - p).cross(r) == 0) { // i.e., if two segments are collinear
            // Check each discrete point on l2 to see whether it intersects with l1
            // By iterating from <q> to <q> + <s> and check whether each point is reached by some <p> + t<r> 
            // where t ∈ [0, 1].
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

            if (result.size() > 0) {
                return (&result);
            }

            return (nullptr);
        } else { // otherwise parallel
            return (nullptr);
        }
    }
}

// Get size of overlapping point set resulting from each line insertion
// Return a map of points -> # of overlaps (need to check what data structure to use)
void getSol1(vector<vectorLine> &lines) {

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

    vector<xyPair> intersectionSet;
    vector<xyPair> * results = intersections(l5, l9, intersectionSet);
    assert(results == nullptr);
    results = intersections(l1, l7, intersectionSet);
    assert(&intersectionSet == results);
}

int main() {
    test();
    return 0;
}

