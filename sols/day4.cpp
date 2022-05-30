#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <cstring>
#include <algorithm>
#include <cassert>
using namespace std;

typedef vector<int> orderType;
typedef vector<vector<int>> boardType;
typedef vector<boardType> boardsType;

pair<orderType, boardsType> parser(string filepath) {
    ifstream inputf (filepath);
    string nextline;
    getline (inputf, nextline); // nextline now has order of selection

    // parse order
    orderType order;
    char* token = strtok(&(nextline[0]), ",");
    while (token) {
        order.push_back(stoi(token));
        token = strtok(nullptr, ",");
    }

    // parse boards
    boardsType boards;
    getline (inputf, nextline); // go to next line, which should be empty
    while (inputf.peek() != EOF) {
        if (nextline.empty()) { // if nextline is empty
            boardType newBoard; // initialize new board
            while (inputf.peek() != EOF) { // until EOF reached, do
                getline (inputf, nextline); // get next line
                if (nextline.empty()) break; // if next line is empty, break to outer loop (board boundary reached)
                vector<int> newRow; // else initialize and parse new row
                char* token = strtok(&(nextline[0]), " ");
                while (token) {
                    newRow.push_back(stoi(token));
                    token = strtok(nullptr, " ");
                }
                newBoard.push_back(newRow); // which is in *this* new board
            }
            boards.push_back(newBoard);
        }
    }

    return pair<orderType, boardsType>(order, boards);
}

typedef tuple<vector<int>, int, long> orderedAdjacency; // vector itself, board it belongs to, fulfilled at order input idx

long getSol1(orderType order, boardsType boards) {
    // Break boards into row / column vectors
    vector<orderedAdjacency> adjacents;
    int boardNum = 0;
    for (boardType board : boards) {
        // Not sure whether bingo assumes square matrix
        // So first add rows
        for (int i = 0; i < board.size(); i++) {
            adjacents.push_back(orderedAdjacency (board[i], boardNum, -1));
        }

        // Then columns
        for (int j = 0; j < board[0].size(); j++) {
            vector<int> nextColumn;
            for (int i = 0; i < board.size(); i++) {
                nextColumn.push_back(board[i][j]);
            }
            adjacents.push_back(orderedAdjacency (nextColumn, boardNum, -1));
        }

        boardNum++;
    }

    // Sort each vector by last element occurrence in order
    // For each vector elem, apply std::find and check returned iterator distance
    // if exists iterator == last then default to INT32_MAX (vectors are not good at removals, as it turns out)
    // This is suboptimal behavior - the entire program honestly should be rewritten w/ matrix-like data structures 
    // but we are too far gone oh well
    int i = 0;
    while (i < adjacents.size()) {
        vector<int> vec = get<0>(adjacents[i]);
        int maxDist = -1;
        for (int p : vec) {
            vector<int>::iterator it = find(order.begin(), order.end(), p);
            if (it == order.end()) {
                maxDist = INT32_MAX;
                break;
            } else if (distance(order.begin(), it) > maxDist) {
                maxDist = distance(order.begin(), it);
            }
        }
        get<2>(adjacents[i]) = maxDist;
        i++;
    }
    sort(adjacents.begin(), adjacents.end(), 
        [](orderedAdjacency a, orderedAdjacency b) {
            return (get<2>(a) < get<2>(b));
        }
    );
    orderedAdjacency minOfMax = adjacents[0];

    // Now that we have 
    int boardIdx = get<1>(minOfMax);
    int split = get<2>(minOfMax) + 1;
    int winningNumber = order[split - 1];
    // Sum all numbers on boards[boardIdx] unless also in order[0:split]
    // This could be improved - we don't need that many variables pointing to basically the same stuff...
    boardType winningBoard = boards[boardIdx];
    vector<int>::iterator subOrderEnd = order.begin() + split;
    int sum = 0;
    for (vector<int> row : winningBoard) {
        for (int entry : row) {
            if (find(order.begin(), subOrderEnd, entry) != subOrderEnd) { // i.e., entry in order[0:split - 1]
                continue;
            }
            sum += entry;
        }
    }

    return (sum * winningNumber);
}


// This implementation is wrong.
// At sorting stage we should get the first winning adjacencies of EACH table to return the losing table.
long getSol2(orderType order, boardsType boards) {
    // Break boards into row / column vectors
    vector<orderedAdjacency> adjacents;
    int boardNum = 0;
    for (boardType board : boards) {
        // Not sure whether bingo assumes square matrix
        // So first add rows
        for (int i = 0; i < board.size(); i++) {
            adjacents.push_back(orderedAdjacency (board[i], boardNum, -1));
        }

        // Then columns
        for (int j = 0; j < board[0].size(); j++) {
            vector<int> nextColumn;
            for (int i = 0; i < board.size(); i++) {
                nextColumn.push_back(board[i][j]);
            }
            adjacents.push_back(orderedAdjacency (nextColumn, boardNum, -1));
        }

        boardNum++;
    }

    // Sort each vector by last element occurrence in order
    // For each vector elem, apply std::find and check returned iterator distance
    // if exists iterator == last then default to INT32_MAX (vectors are not good at removals, as it turns out)
    // This is suboptimal behavior - the entire program honestly should be rewritten w/ matrix-like data structures 
    // but we are too far gone oh well
    int i = 0;
    while (i < adjacents.size()) {
        vector<int> vec = get<0>(adjacents[i]);
        int maxDist = -1;
        for (int p : vec) {
            vector<int>::iterator it = find(order.begin(), order.end(), p);
            if (it == order.end()) {
                maxDist = INT32_MAX;
                break;
            } else if (distance(order.begin(), it) > maxDist) {
                maxDist = distance(order.begin(), it);
            }
        }
        get<2>(adjacents[i]) = maxDist;
        i++;
    }
    sort(adjacents.begin(), adjacents.end(), 
        [](orderedAdjacency a, orderedAdjacency b) {
            return (get<2>(a) < get<2>(b));
        }
    );

    // Now, find the *last* winning col/row of each board

    orderedAdjacency maxOfMax = adjacents[0];

    // Now that we have 
    int boardIdx = get<1>(maxOfMax);
    int split = get<2>(maxOfMax) + 1;
    int losingNumber = order[split - 1];
    // Sum all numbers on boards[boardIdx] unless also in order[0:split]
    // This could be improved - we don't need that many variables pointing to basically the same stuff...
    boardType losingBoard = boards[boardIdx];
    vector<int>::iterator subOrderEnd = order.begin() + split;
    int sum = 0;
    for (vector<int> row : losingBoard) {
        for (int entry : row) {
            if (find(order.begin(), subOrderEnd, entry) != subOrderEnd) { // i.e., entry in order[0:split - 1]
                continue;
            }
            sum += entry;
        }
    }

    return (sum * losingNumber);
}

void test() {
    orderType exampleOrder 
        {7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1};
    boardsType exampleBoards {
        {
            {22, 13, 17, 11, 0}, 
            {8, 2, 23, 4, 24}, 
            {21, 9, 14, 16, 7}, 
            {6, 10, 3, 18, 5}, 
            {1, 12, 20, 15, 19}
        }, 
        { 
            {3, 15, 0, 2, 22}, 
            {9, 18, 13, 17, 5}, 
            {19, 8, 7, 25, 23}, 
            {20, 11, 10, 24, 4}, 
            {14, 21, 16, 12, 6}
        }, 
        {
            {14, 21, 17, 24, 4}, 
            {10, 16, 15, 9, 19}, 
            {18, 8, 23, 26, 20}, 
            {22, 11, 13, 6, 5}, 
            {2, 0, 12, 3, 7}
        }
    };

    pair<orderType, boardsType> parsedResults = parser("../resource/q4/input_test");

    // Parser test
    assert(parsedResults.first == exampleOrder);
    assert(parsedResults.second == exampleBoards);

    // Sol1 test
    long sol1 = getSol1(parsedResults.first, parsedResults.second);
    assert(sol1 == 4512);

    // Sol2 test
    long sol2 = getSol2(parsedResults.first, parsedResults.second);
    assert(sol2 == 1924);

    cout << "Test passed!" << endl;
}

int main() {
    test();
    auto parsedResults = parser("../resource/q4/input");
    cout << getSol1(parsedResults.first, parsedResults.second) << endl;
    return 0;
}


