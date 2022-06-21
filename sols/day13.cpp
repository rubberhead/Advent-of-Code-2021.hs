// Basically, 
// For each fold until the last fold cmd, 
// 1. Filter the set of all dots into two subsets (plz in-place), 
//    one containing dots not folded in and another containing dots folded in
//    (maybe keep in ordered set then find an iterator-like?)
// 2. Apply (fold_threshold - dist_to_threshold) onto each element in folded subset
// 3. Get the disjunction of two subsets for the next fold cmd (either in-place 
//    or with input set deleted from memory

// std::set<T>::value_type is of T const type!
// This implies that all args to lambda exp must be of const type and cannot be mutated. 
// Makes sense considering how tricky and slow it would be to maintain set invariance during each mutation

// Aw shit aw fuck
// I think I misinterpreted the question and made the question a lot harder than it seems

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <set>
#include <vector>
#include <memory>
#include <cassert>
#define toDigit(c) (c - '0')
using namespace std;

// Referred to stackoverflow Q.#7777827 (xtofl)
struct x_cmp {
    bool operator() (pair<int, int> a, pair<int, int> b) const {
        return (a.first < b.first);
    }
};
struct y_cmp {
    bool operator() (pair<int, int> a, pair<int, int> b) const {
        return (a.second < b.second);
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

unique_ptr<xyset> applyFoldCmd(xyset& pts_before_cmd, pair<char, int> const & cmd) {
    char const & direction = cmd.first;
    int const & threshold = cmd.second;
    unique_ptr<xyset> pts_resultant (new xyset());
    if (direction == 'x') {
        // All elems after split in set (w/ x-based idx) must be folded up
        auto split = find_if(pts_before_cmd.xidxs.begin(), pts_before_cmd.xidxs.end(), // O(n)
            [&](pair<int, int> const & xypair) {
                return (xypair.first > threshold);
            }
        );
        // All elems w/ x value before border must be within the return set
        int border_after_fold = threshold - ((*(pts_before_cmd.xidxs.end() - 1)).first - threshold);
        for
        for_each(split, pts_before_cmd.xidxs.end(), // O(n^2)
            [&](pair<int, int> const & xypair) {
                int newfirst = threshold - (xypair.first - threshold);
                pair<int, int> newpair (newfirst, xypair.second);
                if (find(pts_before_cmd.xidxs.begin(), split, newpair) != split) {
                    // folded pt also in unmodified part of original set
                    pts_resultant->insert(newpair);
                } 
            }
        );
        return (pts_resultant);
    }

    if (direction == 'y') {
        auto split = find_if(pts_before_cmd.yidxs.begin(), pts_before_cmd.yidxs.end(), 
            [&](pair<int, int> const & xypair) {
                return (xypair.second > threshold);
            }
        );
        for_each(split, pts_before_cmd.yidxs.end(), 
            [&](pair<int, int> const & xypair) {
                int newsecond = threshold - (xypair.second - threshold);
                pair<int, int> newpair (xypair.first, newsecond);
                if (find(pts_before_cmd.yidxs.begin(), split, newpair) != split) {
                    // folded pt also in unmodified part of original set
                    pts_resultant->insert(newpair);
                } 
            }
        );
        return (pts_resultant);
    }
}

// Assumes empty pts, cmds
void parser(const string& filepath, xyset& pts, vector<pair<char, int>>& cmds) {
    ifstream inputf (filepath);
    string nextline;
    getline (inputf, nextline); // loaded first line (which should be points)
    while (nextline != "\r") { // parse points
        stringstream nlstream (nextline);
        string x, y;
        getline (nlstream, x, ',');
        getline (nlstream, y);
        pts.insert(pair<int, int>(stoi(x), stoi(y)));

        getline (inputf, nextline);
    }

    while (inputf.peek() != EOF) { // parse commands
        getline (inputf, nextline);
        auto parse_pos_dir = nextline.rfind(" ") + 1;
        auto parse_pos_thres = nextline.rfind("=") + 1;
        cmds.push_back(pair<char, int>(nextline[parse_pos_dir], toDigit(nextline[parse_pos_thres])));
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
    return 0;
}