// Classic DP question... Remember IADS?
// This is similar to the seam carving algorithm using dp.
#include <iostream>
#include <fstream>
#include <vector>
#define toDigit(c) (c - '0')
using namespace std;

// Create opt table to every point in space defined by risk function table
// Then backtrack from last elem in opt table until the start
int lowestRiskDP (vector<vector<int>> const& risks) {

    return -1;
}