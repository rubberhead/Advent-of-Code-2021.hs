#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <set>
#include <vector>
#include <deque>
#include <memory>
#include <cassert>
#define toDigit(c) (c - '0')
using namespace std;

// Referred to stackoverflow Q.#7777827 (xtofl)
struct x_cmp {
    bool operator() (pair<int, int> a, pair<int, int> b) const {
        return (a.first <= b.first);
    }
};
struct y_cmp {
    bool operator() (pair<int, int> a, pair<int, int> b) const {
        return (a.second <= b.second);
    }
};
struct xyset {
    set<pair<int, int>, x_cmp> xidxs;
    set<pair<int, int>, y_cmp> yidxs;

    void insert(pair<int, int> p) {
        xidxs.insert(p);
        yidxs.insert(p);
    }
};

unique_ptr<xyset> applyFoldCmd(xyset& original_pts, pair<char, int> const& cmd) {
    char const& direction = cmd.first;
    int const& threshold = cmd.second;
    unique_ptr<xyset> resultant_pts_ptr (new xyset());

    // 1. All points before threshold in original_pts must appear in resultant_pts
    // 2. All points after threshold in original_pts are subtracted w/ (2 * dist_to_threshold) to get 
    //    equivalent position after folding.
    // 3. Attempt to insert them nevertheless. If they are equivalent w/ some point before threshold then 
    //    they cannot get added in the first place.
    if (direction == 'x') {
        for_each(original_pts.xidxs.begin(), original_pts.xidxs.end(), 
            [&](pair<int, int> const& xypair) {
                if (xypair.first < threshold) {
                    resultant_pts_ptr->insert(xypair);
                } else if (xypair.first > threshold) { // comparator guarantees order of iteration
                    pair<int, int> newpair (xypair.first - (2 * (xypair.first - threshold)), xypair.second);
                    if (find(original_pts.xidxs.begin(), original_pts.xidxs.end(), newpair) == original_pts.xidxs.end()){
                        resultant_pts_ptr->insert(newpair);
                    }
                }
            }
        );
        return (resultant_pts_ptr);
    }
    if (direction == 'y') {
        for_each(original_pts.yidxs.begin(), original_pts.yidxs.end(), 
            [&](pair<int, int> const& xypair) {
                if (xypair.second < threshold) {
                    resultant_pts_ptr->insert(xypair);
                } else if (xypair.second > threshold) {
                    pair<int, int> newpair (xypair.first, xypair.second - (2 * (xypair.second - threshold)));
                    if (find(original_pts.yidxs.begin(), original_pts.yidxs.end(), newpair) == original_pts.yidxs.end()){
                        resultant_pts_ptr->insert(newpair);
                    }
                }
            }
        ); 
        return (resultant_pts_ptr);
    }
    throw (invalid_argument("Invalid direction: should be one of x or y"));
}

// Assumes empty pts, cmds
void parser(const string& filepath, xyset& pts, vector<pair<char, int>>& cmds) {
    ifstream inputf (filepath);
    string nextline;
    getline (inputf, nextline); // loaded first line (which should be points)
    while (nextline != "\r" && nextline != "") { // parse points
        stringstream nlstream (nextline);
        string x, y;
        getline (nlstream, x, ',');
        getline (nlstream, y);
        pts.insert(pair<int, int>(stoi(x), stoi(y)));

        getline (inputf, nextline);
    }

    while (inputf.peek() != EOF) { // parse commands
        getline (inputf, nextline);
        stringstream nlstream (nextline);
        string dir, thres;
        // pattern-matching
        getline (nlstream, dir, ' ');
        getline (nlstream, dir, ' ');
        getline (nlstream, dir, '=');
        getline (nlstream, thres);
        cmds.push_back(pair<char, int>(dir[0], stoi(thres)));
    }
}

void test() {
    xyset pts;
    vector<pair<char, int>> cmds;
    parser("../resource/q13/input_test", pts, cmds);

    assert(applyFoldCmd(pts, cmds[0])->xidxs.size() == 17);
}

int main() {
    test();
    xyset pts;
    vector<pair<char, int>> cmds;
    parser("../resource/q13/input", pts, cmds);
    // 1.
    cout << "Part 1: " << applyFoldCmd(pts, cmds[0])->xidxs.size() << endl;

    return 0;
}