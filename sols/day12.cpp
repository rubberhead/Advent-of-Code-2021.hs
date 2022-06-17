#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <sstream>
#include <algorithm>
#include <memory>
#include <cassert>
using namespace std;

typedef unordered_map<string, vector<string>> Connectivity;
typedef unordered_map<string, vector<pair<string, int>>> formattedConnectivity;

unique_ptr<pair<Connectivity, unordered_set<string>>> parser(const string filepath) {
    ifstream inputf (filepath);
    string startnode;
    string endnode;
    unique_ptr<unordered_set<string>> large_caves (new unordered_set<string>());
    unique_ptr<Connectivity> conversion (new Connectivity());

    while (inputf.peek() != EOF) {
        getline (inputf, startnode, '-');
        getline (inputf, endnode);
        // Check small or large cave
        if (isupper(startnode[0])) {
            large_caves->insert(startnode);
        }
        if (isupper(endnode[0])) {
            large_caves->insert(endnode);
        }

        // Add to connectivity "list"
        try {
            conversion->at(startnode).push_back(endnode);
        } catch (out_of_range) {
            (*conversion)[startnode] = {endnode};
        }
        try {
            conversion->at(endnode).push_back(startnode);
        } catch (out_of_range) {
            (*conversion)[endnode] = {startnode};
        }
    }

    unique_ptr<pair<Connectivity, unordered_set<string>>> returnval 
        (new pair<Connectivity, unordered_set<string>>(*(conversion), *(large_caves)));
    return (returnval);    
}

int getSol1(Connectivity& conversion, const unordered_set<string>& large_caves) {
    // Perform BFS from "start", keep track of available route choices
    // If BFS finds "end", add to counter (which is to be returned)
    int counter = 0;
    queue<vector<string>> fifo_paths; 
    fifo_paths.push({"start"});
    while (!fifo_paths.empty()) {
        vector<string> currpath = fifo_paths.front();
        string lastnode = currpath[currpath.size() - 1];
        fifo_paths.pop();

        if (lastnode == "end") {
            counter ++;
        } else {
            // otherwise, if not backwards then a new path
            for (string nextnode : conversion[lastnode]) {
                if (find(large_caves.begin(), large_caves.end(), nextnode) != large_caves.end() || // nextnode a large cave
                    find(currpath.begin(), currpath.end(), nextnode) == currpath.end()) { // or nextnode (as small node) not visited before
                    vector<string> nextpath (currpath);
                    nextpath.push_back(nextnode);
                    fifo_paths.push(nextpath);
                }
            }
        }
    }

    return counter;
}

void test() {
    auto kvpair = *(parser("../resource/q12/input_test"));
    auto connectivity = kvpair.first;
    auto large_caves = kvpair.second;
    assert(large_caves.size() == 1);
    assert(getSol1(connectivity, large_caves) == 10);

    kvpair = *(parser("../resource/q12/input_test1"));
    connectivity = kvpair.first;
    large_caves = kvpair.second;
    assert(getSol1(connectivity, large_caves) == 19);

    kvpair = *(parser("../resource/q12/input_test2"));
    connectivity = kvpair.first;
    large_caves = kvpair.second;
    assert(getSol1(connectivity, large_caves) == 226);
}

int main() {
    test();
    auto kvpair = *(parser("../resource/q12/input"));
    auto connectivity = kvpair.first;
    auto large_caves = kvpair.second;

    // 1.
    cout << "Part 1: " << getSol1(connectivity, large_caves) << endl;


    return 0;
}

