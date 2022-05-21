// C++ time
#include <iostream>
#include <fstream>
using namespace std;

// Part 1
// Objective is to return multiplication of sum of final horizontal and depth pos.
int getSol1(string filepath) {
    int x_sum = 0;
    int y_sum = 0;
    ifstream inputf (filepath); // defaults to in

    string nextline;
    getline (inputf, nextline); // first line

    while (nextline != "") { // tokenize while not at EOF
        size_t split = nextline.find(" ");
        string direction = nextline.substr(0, split);
        int amount = 0;
        try {
            amount = stoi(nextline.substr(split + 1));
        } catch (invalid_argument& e) {
            cerr << nextline.substr(split + 1) << e.what() << endl;
            return -1;
        }

        //cout << x_sum << " " << y_sum << endl;
        if (direction == "forward") {
            x_sum += amount;
        } else if (direction == "down") {
            y_sum += amount;
        } else if (direction == "up") {
            y_sum -= amount;
        } else {
            throw invalid_argument("Undefined direction found in input file.");
        }

        getline (inputf, nextline); // update line
    }

    inputf.close();

    // At EOF
    return (x_sum * y_sum);
}

// Part 2
// Like above but keeps track of aim in addition
long getSol2(string filepath) {
    long x_sum = 0;
    long y_sum = 0;
    long aim = 0;
    ifstream inputf (filepath); // defaults to in

    string nextline;
    getline (inputf, nextline); // first line

    while (nextline != "") { // tokenize while not at EOF
        size_t split = nextline.find(" ");
        string direction = nextline.substr(0, split);
        int amount = 0;
        try {
            amount = stoi(nextline.substr(split + 1));
        } catch (invalid_argument& e) {
            cerr << nextline.substr(split + 1) << e.what() << endl;
            return -1;
        }

        cout << x_sum << " " << y_sum << " " << aim << endl;
        if (direction == "forward") {
            x_sum += amount;
            y_sum += amount * aim;
        } else if (direction == "down") {
            aim += amount;
        } else if (direction == "up") {
            aim -= amount;
        } else {
            throw invalid_argument("Undefined direction found in input file.");
        }

        getline (inputf, nextline); // update line
    }
    
    inputf.close();

    // At EOF
    return (x_sum * y_sum);
}



int main() {
    cout << getSol1("../resource/q2/input") << endl;
    cout << getSol2("../resource/q2/input") << endl;
    return 0;
}