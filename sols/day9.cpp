#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cassert>
#define toDigit(c) (c - '0')
using namespace std;

int getSol1(vector<vector<int>> const &height_map) {
    int risk_sum = 0;
    for (int i = 0; i < height_map.size(); i++) {
        for (int j = 0; j < height_map[i].size(); j++) {
            vector<int> comps {height_map[i][j]};
            if (i - 1 >= 0) {
                comps.push_back(height_map[i - 1][j]);
            }
            if (j - 1 >= 0) {
                comps.push_back(height_map[i][j - 1]);
            }
            if (i + 1 < height_map.size()) {
                comps.push_back(height_map[i + 1][j]);
            }
            if (j + 1 < height_map[i].size()) {
                comps.push_back(height_map[i][j + 1]);
            }

            if (*(min_element(comps.begin() + 1, comps.end())) > *(comps.begin())) {
                // First elem of comps is always the center elem itself
                risk_sum += (comps[0] + 1);
            }
        }
    }
    return risk_sum;
}

void getAdjacents(int const i, int const j, vector<vector<int>> const &height_map, vector<int> &comps) {
    if (i - 1 >= 0) {
        comps.push_back(height_map[i - 1][j]);
    }
    if (j - 1 >= 0) {
        comps.push_back(height_map[i][j - 1]);
    }
    if (i + 1 < height_map.size()) {
        comps.push_back(height_map[i + 1][j]);
    }
    if (j + 1 < height_map[i].size()) {
        comps.push_back(height_map[i][j + 1]);
    }
}

// Expand frontier to neighbouring entries that: 
// - Are within limit
// - Are increasing
// - Are below 9 in value
// - Have not yet been inserted before
void getGoodAdjacents(
    int const i, 
    int const j, 
    vector<vector<int>> const &height_map, 
    set<pair<int, int>> &adjacents) {
    if (i - 1 >= 0) {
        if (height_map[i - 1][j] < 9 && 
            height_map[i - 1][j] > height_map[i][j]) {
            adjacents.insert(pair<int, int> (i - 1, j));
        }
    }
    if (j - 1 >= 0) {
        if (height_map[i][j - 1] < 9 && 
            height_map[i][j - 1] > height_map[i][j]) {
            adjacents.insert(pair<int, int> (i, j - 1));
        }
    }
    if (i + 1 < height_map.size()) {
        if (height_map[i + 1][j] < 9 && 
            height_map[i + 1][j] > height_map[i][j]) {
            adjacents.insert(pair<int, int> (i + 1, j));
        }
    }
    if (j + 1 < height_map[i].size()) {
        if (height_map[i][j + 1] < 9 && 
            height_map[i][j + 1] > height_map[i][j]) {
            adjacents.insert(pair<int, int> (i, j + 1));
        }
    }
}

int getSol2(vector<vector<int>> const &height_map) {
    // Upon finding one low point, 
    // perform BFS centered on that point radially in x/y directions
    // until either increasing stops or 9 reached or border reached
    // Count the number of nodes traversed in the process (which is the basin size)
    // Store that number in a priority queue-like structure
    priority_queue<int> sizes;
    for (int i = 0; i < height_map.size(); i++) {
        for (int j = 0; j < height_map[i].size(); j++) {
            vector<int> comps {height_map[i][j]};
            getAdjacents(i, j, height_map, comps);

            if (*(min_element(comps.begin() + 1, comps.end())) > *(comps.begin())) {
                // First elem of comps is always the center elem itself
                // In this case a low point is found at height_map[i][j]
                int curr_size = 0;
                set<pair<int, int>> frontier;
                getGoodAdjacents(i, j, height_map, frontier);
                int next_size = curr_size + frontier.size();

                // Get adjacents until size cannot change 
                // A more optimized option would be to keep two sets storing prev and next frontier
                // And updating them bootstrapping-ly.
                while (next_size > curr_size) {
                    curr_size = next_size;
                    for_each(frontier.begin(), frontier.end(), 
                        [&] (pair<int, int> ijPair) {
                            getGoodAdjacents(ijPair.first, ijPair.second, height_map, frontier);
                        }
                    );
                    next_size = frontier.size();
                }

                sizes.push(curr_size + 1); // Including [i][j] itself
            }
        }
    }
    int max = sizes.top();
    sizes.pop();
    int snd = sizes.top();
    sizes.pop();
    int trd = sizes.top();
    return (max * snd * trd);
}

void parser(string const filepath, vector<vector<int>> &height_map) {
    ifstream inputf (filepath);
    string nextline;
    int idx = height_map.size();
    while (inputf.peek() != EOF) {
        height_map.push_back({});
        getline (inputf, nextline);
        for (char c : nextline) {
            height_map[idx].push_back(toDigit(c));
        }
        idx ++;
    }
}

void test() {
    vector<vector<int>> example {
        {2,1,9,9,9,4,3,2,1,0}, 
        {3,9,8,7,8,9,4,9,2,1}, 
        {9,8,5,6,7,8,9,8,9,2}, 
        {8,7,6,7,8,9,6,7,8,9}, 
        {9,8,9,9,9,6,5,6,7,8}
    };
    vector<vector<int>> parsed;
    parser("../resource/q9/input_test", parsed);
    assert(parsed == example);
    assert(getSol1(example) == 15);
    assert(getSol2(example) == 1134);
}

int main() {
    test();
    vector<vector<int>> height_map;
    parser("../resource/q9/input", height_map);
    // 1.
    cout << "Part 1: " << getSol1(height_map) << endl;

    // 2.
    cout << "Part 2: " << getSol2(height_map) << endl;

    return 0;
}