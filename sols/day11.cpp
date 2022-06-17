// Another simulation problem...
// Unlike day 5, however, this problem requires simulation of each entity
// Struggled with bugs, ended up copying code online
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <set>
#define toDigit(c) (c - '0')
using namespace std;

// Takes reference to initial state of energy level of each octopus in universe
// Destructive.
int simulate(vector<vector<int>>& initial_state, int step) {
    int flash_count = 0;
    for (int t = 1; t <= step; t++) {
        set<pair<int, int>> ready_locs;
        int curr_flash_count = 0;
        // Insert currently new ready pairs into set
        for (int r = 0; r < initial_state.size(); r++) {
            for (int c = 0; c < initial_state[r].size(); c++) {
                initial_state[r][c] ++;
                if (initial_state[r][c] > 9) {
                    ready_locs.insert(pair(r, c));
                }
            }
        }
        // Expand on frontier until nothing could be expanded on further
        // This section is mostly referred to from online materials
        while (ready_locs.size() > 0) {
            auto this_ready = ready_locs.begin();
            ready_locs.erase(this_ready);
            int i = this_ready->first;
            int j = this_ready->second;
            initial_state[i][j] = 0;
            curr_flash_count ++;
            for (int r = i - 1; r <= i + 1; r++) {
                for (int c = j - 1; c <= j + 1; c++) {
                    if (r >= 0 && r < initial_state.size() && 
                        c >= 0 && c < initial_state[r].size()) {
                        if (initial_state[r][c] > 0 && initial_state[r][c] < 10) { // not flashed yet
                            initial_state[r][c] ++;
                        }
                        if (initial_state[r][c] > 9) {
                            ready_locs.insert(pair(r, c));
                        }
                    }
                }
            }
        }
        
        flash_count += curr_flash_count;
    }
    return flash_count;
}

int getSol1(vector<vector<int>>& initial_state, int step) {
    int result = simulate(initial_state, step);
    return (result);
}

int getSol2(vector<vector<int>>& initial_state) {
    const int ALL_FLASH_COUNT = 100;
    int curr_flash_count = 0;
    int t = 0;
    while (curr_flash_count != ALL_FLASH_COUNT) {
        curr_flash_count = 0;
        ++t;

        set<pair<int, int>> ready_locs;
        // Insert currently new ready pairs into set
        for (int r = 0; r < initial_state.size(); r++) {
            for (int c = 0; c < initial_state[r].size(); c++) {
                initial_state[r][c] ++;
                if (initial_state[r][c] > 9) {
                    ready_locs.insert(pair(r, c));
                }
            }
        }
        // Expand on frontier until nothing could be expanded on further
        // This section is mostly referred to from online materials
        while (ready_locs.size() > 0) {
            auto this_ready = ready_locs.begin();
            ready_locs.erase(this_ready);
            int i = this_ready->first;
            int j = this_ready->second;
            initial_state[i][j] = 0;
            curr_flash_count ++;
            for (int r = i - 1; r <= i + 1; r++) {
                for (int c = j - 1; c <= j + 1; c++) {
                    if (r >= 0 && r < initial_state.size() && 
                        c >= 0 && c < initial_state[r].size()) {
                        if (initial_state[r][c] > 0 && initial_state[r][c] < 10) { // not flashed yet
                            initial_state[r][c] ++;
                        }
                        if (initial_state[r][c] > 9) {
                            ready_locs.insert(pair(r, c));
                        }
                    }
                }
            }
        }
    }
    return t; // if curr_flash_count == 100, exit out loop and return t
}

void parser(const string& filepath, vector<vector<int>>& empty_state) {
    ifstream inputf (filepath);
    string nextline;
    int idx = 0;
    while (inputf.peek() != EOF) {
        empty_state.push_back({});
        getline (inputf, nextline);
        for (char c : nextline) {
            empty_state[idx].push_back(toDigit(c));
        }  
        idx ++;
    }
}

void test() {
    vector<vector<int>> test_0 = {
        {1,1,1,1,1}, 
        {1,9,9,9,1}, 
        {1,9,1,9,1}, 
        {1,9,9,9,1}, 
        {1,1,1,1,1}
    };
    vector<vector<int>> test_1 = {
        {5,4,8,3,1,4,3,2,2,3}, 
        {2,7,4,5,8,5,4,7,1,1}, 
        {5,2,6,4,5,5,6,1,7,3}, 
        {6,1,4,1,3,3,6,1,4,6}, 
        {6,3,5,7,3,8,5,4,7,8}, 
        {4,1,6,7,5,2,4,6,4,5}, 
        {2,1,7,6,8,4,1,7,2,1}, 
        {6,8,8,2,8,8,1,1,3,4}, 
        {4,8,4,6,8,4,8,5,5,4}, 
        {5,2,8,3,7,5,1,5,2,6}
    };

    assert(getSol1(test_0, 2) == 9);
    assert(getSol1(test_1, 10) == 204);
    assert(getSol2(test_1) == (195 - 10));
}

int main() {
    test();
    vector<vector<int>> states;
    parser("../resource/q11/input", states);

    // 1.
    cout << "Part 1: " << getSol1(states, 100) << endl;
    
    // 2.
    states = {};
    parser("../resource/q11/input", states);
    cout << "Part 2: " << getSol2(states) << endl;
    return 0;
}

