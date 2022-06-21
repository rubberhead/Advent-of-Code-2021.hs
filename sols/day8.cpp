#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <cmath>
using namespace std;

// Unique
const int ONE_SEGMENTS = 2;
const int FOUR_SEGMENTS = 4;
const int SEVEN_SEGMENTS = 3;
const int EIGHT_SEGMENTS = 7;

// Not unique
const int ZERO_SEGMENTS = 6;
const int TWO_SEGMENTS = 5;
const int THREE_SEGMENTS = 5;
const int FIVE_SEGMENTS = 5;
const int SIX_SEGMENTS = 6;
const int NINE_SEGMENTS = 6;

// Takes concatenated outputs among each entry
int getSol1(vector<string> &concat_outputs) {
    int result = count_if(concat_outputs.begin(), concat_outputs.end(), 
        [](string s) {
            return (
                s.length() == ONE_SEGMENTS || 
                s.length() == FOUR_SEGMENTS ||
                s.length() == SEVEN_SEGMENTS || 
                s.length() == EIGHT_SEGMENTS
            );
        }
    );
    return result;
}

// Helper function.
// Checks whether string a's characters are a subset of b's.
bool contained_by(string const &a, string const &b) {
    for (char ax : a) {
        if (find(b.begin(), b.end(), ax) == b.end()) {
            return false;
        }
    }
    return true;
}

// This could be solved via DPLL or something more specific
// This is a specific variant that looks quite ugly honestly: vectors don't handle deletion well.
void decode(vector<string> &patterns, unordered_map<string, int> &result) { // 10 patterns from 0 to 9
    // First assign strings w/ unique lengths
    vector<string> assigned;
    string one_pattern, four_pattern, seven_pattern, eight_pattern;
    for (string pattern : patterns) {
        switch (pattern.length()) {
            case ONE_SEGMENTS : 
                result[pattern] = 1;
                one_pattern = pattern;
                assigned.push_back(one_pattern);
                break;
            case FOUR_SEGMENTS :
                result[pattern] = 4;
                four_pattern = pattern;
                assigned.push_back(four_pattern);
                break;
            case SEVEN_SEGMENTS :
                result[pattern] = 7;
                seven_pattern = pattern;
                assigned.push_back(seven_pattern);
                break;
            case EIGHT_SEGMENTS :
                result[pattern] = 8;
                eight_pattern = pattern;
                assigned.push_back(eight_pattern);
                break;
            default :
                continue; // or sth else?
        }
    }

    // Then assign 0, 3, 9
    string zero_pattern, three_pattern, nine_pattern, six_pattern, five_pattern, two_pattern;
    for (string pattern : patterns) {
        // 0 contains segments of 7 and has length 6
        // 3 contains segments of 7 and has length 5
        // 9 contains segments of both 7 and 4 (and has length 6)
        if (result.find(pattern) == result.end()) {
            if (contained_by(seven_pattern, pattern) && contained_by(four_pattern, pattern)) {
                result[pattern] = 9;
                nine_pattern = pattern;
                continue;
            } 
            if (contained_by(seven_pattern, pattern) && pattern.size() == 6) {
                result[pattern] = 0;
                zero_pattern = pattern;
                continue;
            } 
            if (contained_by(seven_pattern, pattern) && pattern.size() == 5) {
                result[pattern] = 3;
                three_pattern = pattern;
                continue;
            } 
        }
    }

    // Finally assign 6, 5, 2
    for (string pattern : patterns) {
        if (result.find(pattern) == result.end()) {
            if (pattern.size() == 6) {
                result[pattern] = 6;
                six_pattern = pattern;
                continue;
            }
            if (contained_by(pattern, nine_pattern) && pattern.size() == 5) {
                result[pattern] = 5;
                five_pattern = pattern;
                continue;
            } else if (pattern.size() == 5) {
                result[pattern] = 2;
                two_pattern = pattern;
                continue;
            }
        }
    }
}

int getSol2(vector<vector<string>> &patterns, vector<vector<string>> &outputs) {
    int sum = 0;
    for (int i = 0; i < patterns.size(); i++) { // for each pattern-output pair
        vector<string>* pattern = &(patterns[i]);
        vector<string>* output = &(outputs[i]);
        unordered_map<string, int> encodings;
        decode(*pattern, encodings);
        assert (encodings.size() == 10);
        string reading; // converted to int after stoi, better formatting
        for (string &out : *output) {
            // contained_by && equal size => same digit
            auto it = find_if(encodings.begin(), encodings.end(), 
                [&] (auto &kvpair) {
                    return (contained_by(out, kvpair.first) && (kvpair.first.size() == out.size()));
                }
            );
            reading.append(to_string(it->second));
        }
        sum += stoi(reading);
    }
    return sum;
}

void parser(string const filepath, vector<vector<string>> &patterns, vector<vector<string>> &outputs) {
    ifstream inputf (filepath);
    string each_pattern_str;
    string each_output_str;
    int idx = 0;
    while (inputf.peek() != EOF) {
        getline (inputf, each_pattern_str, '|');
        getline (inputf, each_output_str);
        // Initialize new entries in patterns and outputs, pointed to by idx
        patterns.push_back({});
        outputs.push_back({});
        // tokenize pattern
        vector<string>* pattern_tokens = &(patterns[idx]);
        stringstream each_pattern_stream(each_pattern_str); 
        string temp;
        while (getline (each_pattern_stream, temp, ' ')) {
            if (temp.length() > 0) {
                pattern_tokens->push_back(temp);
            }
        }
        // tokenize output
        vector<string>* output_tokens = &(outputs[idx]);
        stringstream each_output_stream(each_output_str);
        while (getline (each_output_stream, temp, ' ')) {
            if (temp.length() > 0) {
                output_tokens->push_back(temp);
            }
        }
        
        idx ++;
    }
}

void test() {
    vector<string> test_output_2 {"cdfeb", "fcadb", "cdfeb", "cdbaf"};
    vector<string> test_output_1 {"fdgacbe", "cefdb", "cefbgd", "gcbe"};
    assert(getSol1(test_output_2) == 0);
    assert(getSol1(test_output_1) == 2);

    vector<vector<string>> patterns;
    vector<vector<string>> outputs;
    vector<string> test_pattern_1 {"be", "cfbegad", "cbdgef", "fgaecd", "cgeb", "fdcge", "agebfd", "fecdb", "fabcd", "edb"};
    parser("../resource/q8/input_test", patterns, outputs);
    assert(patterns[0] == test_pattern_1);
    assert(outputs[0] == test_output_1);
    int result = 0;
    for (vector<string> &opts : outputs) {
        result += getSol1(opts);
    }
    assert(result == 26);

    assert(getSol2(patterns, outputs) == 61229);
}

int main() {
    test();
    vector<vector<string>> patterns;
    vector<vector<string>> outputs;
    parser("../resource/q8/input", patterns, outputs);
    // 1.
    int result = 0;
    for (vector<string> &opts : outputs) {
        result += getSol1(opts);
    }
    cout << "Part 1: " << result << endl;

    // 2.
    result = getSol2(patterns, outputs);
    cout << "Part 2: " << result << endl;
    return 0;
}