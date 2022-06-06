// Alternative solution that sacrifices space (and time, really) but is quite apparent
// Linalg obv. does wonders for speed but it seems my linalg is actually bad... or maybe there's definition faults?
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include <numeric>
using namespace std;

typedef pair<int, int> xyPoint;

map<xyPoint, int> * parser(string filepath, map<xyPoint, int>& points, bool vertical_horizontal_only = false) {
    ifstream inputf (filepath);
    string nextline;
    
    while (inputf.peek() != EOF) {
        getline (inputf, nextline);
        // Parse first and second lines
        // Assuming each line is structured as x,y -> a,b
        char* token_1 = strtok(&(nextline[0]), ",");
        char* token_2 = strtok(nullptr, " ");

        strtok(nullptr, " "); // Pattern-matching
        char* token_3 = strtok(nullptr, ",");
        char* token_4 = strtok(nullptr, ","); // last char so whatever limiter helps really

        int x = stoi(token_1);
        int y = stoi(token_2);
        int a = stoi(token_3);
        int b = stoi(token_4);
        if (vertical_horizontal_only && 
            (x != a && y != b)) {
            continue; 
        }

        xyPoint p (x, y);
        xyPoint q (a, b);
        xyPoint d ((a - x) / gcd(a - x, b - y), (b - y) / gcd(a - x, b - y));
        // Iterate p till q
        // Get direction vector somehow...
        p.first -= d.first;
        p.second -= d.second;
        while (p != q) {
            p.first += d.first;
            p.second += d.second;
            try {
                points.at(p)++;
            } catch (out_of_range) {
                points[p] = 1;
            }
        }
    }
    return (&points);
}

int main() {
    map<xyPoint, int> points;
    map<xyPoint, int> filtered;
    parser("../resource/q5/input", points); // add true param for sol1
    copy_if(points.begin(), points.end(), inserter(filtered, filtered.end()), 
        [](auto const& kv) {
            return (kv.second >= 2);
        }
    );
    cout << filtered.size() << endl;
    return 0;
}