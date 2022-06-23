// This looks similar to the seam carving algorithm using dp, 
// But it's not! Notice we can move left, right, up, and down, which violates
// DP principle of ordered subproblems.
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>
#define toDigit(c) (c - '0')
using namespace std;

// Create opt table to every point in space defined by risk function table
// At the end the bottom right entry should have lowest total risk for reaching that entry
// Turns out incorrect except for this particular test input
int lowestRiskDP (vector<vector<int>> const& risks) {
    vector<vector<int>> opts (
        risks.size(), 
        vector<int>(risks[0].size(), 0)
    ); // Assumes each row of same size in risks, each entry is now 0
    
    // initialize opts: 
    opts[0][0] = 0; // "never entered"
    for (int i = 1; i < risks.size(); i++) { // first column
        opts[i][0] = opts[i - 1][0] + risks[i][0];
    }
    for (int j = 1; j < risks[0].size(); j++) { // first row
        opts[0][j] = opts[0][j - 1] + risks[0][j]; // can only go right
    }
    
    // DP
    for (int i = 1; i < opts.size(); i++) {
        for (int j = 1; j < opts.size(); j++) {
            opts[i][j] = min(opts[i - 1][j], opts[i][j - 1]) + risks[i][j];
        }
    }

    int lowest_total_risk = *(opts.rbegin()->rbegin());
    return (lowest_total_risk);
}

// Get adjacency for each x-y coordinate (i.e., i-j index pair)
// based on their relative position in risks function table.
// O(1).
unique_ptr<vector<pair<int, int>>> adjacents(pair<int, int> const& xypair, vector<vector<int>> const& risks) {
    int col_min = 0;
    int col_max = risks[0].size() - 1;
    int row_min = 0;
    int row_max = risks.size() - 1;
    int i = xypair.first;
    int j = xypair.second;
    vector<pair<int, int>> adjacent_indices;
    if (i > row_min) {
        adjacent_indices.push_back(pair<int, int>(i - 1, j));
    }
    if (i < row_max) {
        adjacent_indices.push_back(pair<int, int>(i + 1, j));
    }
    if (j > col_min) {
        adjacent_indices.push_back(pair<int, int>(i, j - 1));
    }
    if (j < col_max) {
        adjacent_indices.push_back(pair<int, int>(i, j + 1));
    }
    return (make_unique<vector<pair<int, int>>>(adjacent_indices));
}

// Graph search algorithm for finding single-source shortest path(s)
// Like Dijkstra since no negative weight exists
int lowestRiskDijkstra (vector<vector<int>> const& risks) {
    // Initialize single source
    vector<vector<int>> dists (
        risks.size(), 
        vector<int>(risks[0].size(), INT32_MAX) // not INF!
    );
    dists[0][0] = 0;
    // Initialize vector storing current fringe vertices (supposed to be max-heap)
    vector<pair<int, int>> frontier ({pair<int, int>(0, 0)});

    // Update dist until graph traversed completely from [0][0]
    while (!frontier.empty()) {
        pair<int, int> ijpair = frontier.back(); // returns min element
        int& i = ijpair.first;
        int& j = ijpair.second;
        auto adjacents_to_ij = *(adjacents(ijpair, risks));
        frontier.pop_back(); 
        for (auto& xypair : adjacents_to_ij) { // (i,j) -> (x,y)
            // RELAX subroutine
            int& x = xypair.first;
            int& y = xypair.second;
            if (dists[x][y] == INT32_MAX) { // First time encountering (x,y)
                dists[x][y] = dists[i][j] + risks[x][y];
                frontier.push_back(xypair);
                push_heap(frontier.begin(), frontier.end(), 
                    [&](pair<int, int> const& pair_a, pair<int, int> const& pair_b) {
                        int x_a = pair_a.first; // Lambda closure allows for calling local variables
                        int x_b = pair_b.first; // Maybe there are better ways?
                        int y_a = pair_a.second;
                        int y_b = pair_b.second;
                        if (dists[x_a][y_a] < dists[x_b][y_b]) {
                            return true;
                        }
                        return false;
                    }
                );
            } 
            if (dists[x][y] > dists[i][j] + risks[x][y]) { // Better route found for (0,0) ~> (i,j) -> (x,y)
                dists[x][y] = dists[i][j] + risks[x][y];
                // C++ STL has no heapify functionalities as defined in CLRS
                // Alternative would be to delete and re-push (x,y) from frontier
                // If done correctly that would be O(3log(n)), but reference to (x,y) in frontier is required somehow...
                // Comparatively, make_heap runs in O(3n) time.
                make_heap(frontier.begin(), frontier.end(), 
                    [&](pair<int, int> const& pair_a, pair<int, int> const& pair_b) {
                        int x_a = pair_a.first; 
                        int x_b = pair_b.first; 
                        int y_a = pair_a.second;
                        int y_b = pair_b.second;
                        if (dists[x_a][y_a] < dists[x_b][y_b]) {
                            return true;
                        }
                        return false;
                    }
                );
            }
        }
    }

    int result = *(dists.rbegin()->rbegin());
    return result;
}


unique_ptr<vector<vector<int>>> parser(string const& filepath) {
    ifstream inputf (filepath);
    string nextline;
    vector<vector<int>> risks;
    int i = 0;
    while (inputf.peek() != EOF) {
        getline(inputf, nextline);
        risks.push_back({});
        for (char c : nextline) {
            risks[i].push_back(c - '0');
        }
        i++;
    }
    return (make_unique<vector<vector<int>>>(risks));
}

void test() {
    vector<vector<int>> risks_test = {
        {1,1,6,3,7,5,1,7,4,2}, 
        {1,3,8,1,3,7,3,6,7,2}, 
        {2,1,3,6,5,1,1,3,2,8}, 
        {3,6,9,4,9,3,1,5,6,9}, 
        {7,4,6,3,4,1,7,1,1,1}, 
        {1,3,1,9,1,2,8,1,3,7}, 
        {1,3,5,9,9,1,2,4,2,1}, 
        {3,1,2,5,4,2,1,6,3,9}, 
        {1,2,9,3,1,3,8,5,2,1}, 
        {2,3,1,1,9,4,4,5,8,1}
    };

    assert(lowestRiskDP(risks_test) == 40);
    assert(lowestRiskDijkstra(risks_test) == 40);
}

int main() {
    test();
    vector<vector<int>> const risks = *(parser("../resource/q15/input"));

    // 1.
    cout << "Part 1: " << lowestRiskDP(risks) << endl;

    return 0;
}