// This looks similar to the seam carving algorithm using dp, 
// But it's not! Notice we can move left, right, up, and down, which violates
// DP principle of ordered subproblems.
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
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
    int col_max = risks[0].size();
    int row_min = 0;
    int row_max = risks.size();
    int i = xypair.first;
    int j = xypair.second;
    vector<pair<int, int>> adjacent_indices;
    if (i > row_min) {
        adjacent_indices.push_back(pair<int, int>(i - 1, j));
    }
    if (i + 1 < row_max) {
        adjacent_indices.push_back(pair<int, int>(i + 1, j));
    }
    if (j > col_min) {
        adjacent_indices.push_back(pair<int, int>(i, j - 1));
    }
    if (j + 1 < col_max) {
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
    // Custom comparator lambda exp
    // Use decltype when passed as type declaration in container creations
    auto frontier_cmp = [&](pair<int, int> const& pair_a, pair<int, int> const& pair_b) {
        if (dists[pair_a.first][pair_a.second] > dists[pair_b.first][pair_b.second]) {
            return true;
        }
        return false;
    };

    // Initialize priority_queue storing current fringe vertices (supposed to be max-heap)
    priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(frontier_cmp)> frontier{frontier_cmp};
    frontier.push(pair<int, int>(0, 0));

    // Update dist until graph traversed completely from [0][0]
    while (!frontier.empty()) {
        pair<int, int> ijpair = frontier.top(); // returns min element
        int i = ijpair.first;
        int j = ijpair.second;
        auto adjacents_to_ij = *(adjacents(ijpair, risks)); // up, down, left, right at max
        frontier.pop();
        for (auto& xypair : adjacents_to_ij) { // (i,j) -> (x,y)
            // RELAX subroutine
            int x = xypair.first;
            int y = xypair.second;
            if (dists[i][j] + risks[x][y] < dists[x][y]) { // Better route found that is (0,0) ~> (i,j) -> (x,y)
                dists[x][y] = dists[i][j] + risks[x][y];
                frontier.push(xypair);
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
    cout << "Part 1: " << lowestRiskDijkstra(risks) << endl;

    return 0;
}