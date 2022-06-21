// Basically, 
// For each fold until the last fold cmd, 
// 1. Filter the set of all dots into two subsets (plz in-place), 
//    one containing dots not folded in and another containing dots folded in
//    (maybe keep in ordered set then find an iterator-like?)
// 2. Apply (fold_threshold - dist_to_threshold) onto each element in folded subset
// 3. Get the disjunction of two subsets for the next fold cmd (either in-place 
//    or with input set deleted from memory

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

unique_ptr<set<pair<int, int>>> applyFoldCmd(set<pair<int, int>>& pts_before_cmd, const pair<char, int>& cmd) {
    const char& direction = cmd.first;
    const int& threshold = cmd.second;
    unique_ptr<set<pair<int, int>>> pts_resultant (new set<pair<int, int>>());
    if (direction == 'x') {
        const auto split = find_if(pts_before_cmd.begin(), pts_before_cmd.end(), 
            [&](pair<int, int>& xypair) {
                return (xypair.first > threshold);
            }
        );
        for_each(split, pts_before_cmd.end(), 
            [&](pair<int, int>& xypair) {
                xypair.first = threshold - (xypair.first - threshold);
                if (find(pts_before_cmd.begin(), split, xypair) != split) {
                    // folded pt also in unmodified part of original set
                    pts_resultant->insert(xypair);
                } 
            }
        );
        return (pts_resultant);
    }

    if (direction == 'y') {
        const auto split = find_if(pts_before_cmd.begin(), pts_before_cmd.end(), 
            [&](pair<int, int>& xypair) {
                return (xypair.second > threshold);
            }
        );
        for_each(split, pts_before_cmd.end(), 
            [&](pair<int, int>& xypair) {
                xypair.second = threshold - (xypair.second - threshold);
                if (find(pts_before_cmd.begin(), split, xypair) != split) {
                    // folded pt also in unmodified part of original set
                    pts_resultant->insert(xypair);
                } 
            }
        );
        return (pts_resultant);
    }
}

// Assumes empty pts, cmds
void parser(const string& filepath, set<pair<int, int>>& pts, vector<pair<char, int>>& cmds) {
    ifstream inputf (filepath);
    string nextline;
    getline (inputf, nextline); // loaded first line (which should be points)
    while (nextline != "") { // parse points
        stringstream nlstream (nextline);
        string x, y;
        getline (nlstream, x, ',');
        getline (nlstream, y);
        pts.insert(stoi(x), stoi(y));

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
    set<pair<int, int>> pts;
    vector<pair<char, int>> cmds;
    parser("../resource/q13/input_test", pts, cmds);

    assert(applyFoldCmd(pts, cmds[0])->size() == 17);

}

int main() {
    test();
    return 0;
}