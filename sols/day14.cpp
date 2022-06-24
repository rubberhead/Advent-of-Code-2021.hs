#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <cassert>
using namespace std;

// Naive solution would be to simulate, which works well when step == 10
// But most definitely catches fire when step == 40
// Polymerize given polymer and return unique_ptr to resultant polymer
unique_ptr<string> polymerize(string const& polymer, unordered_map<string, char> const& rules) {
    string result;
    for (long i = 1; i < polymer.size(); i++) {
        string selection = {polymer[i - 1], polymer[i]};
        try {
            char rhs = rules.at(selection);
            selection.insert(1, 1, rhs);
        } catch (out_of_range) {
            throw (invalid_argument("polymer not sufficiently defined by given rules."));
        }
        result.append(selection.substr(1));
    }
    result.insert(0, 1, polymer[0]);
    return (make_unique<string>(result));
}

// Count occurrence of each unique char in a string
// Return a polonger to unordered_map containing this characterization
unique_ptr<unordered_map<char, long>> countCharOccurrence(string const& input) {
    unordered_map<char, long> result;
    for (char c : input) {
        try {
            result.at(c)++;
        } catch (out_of_range) {
            result[c] = 1;
        }
    }
    return (make_unique<unordered_map<char, long>>(result));
}

long getSol1(long step, string const& original_polymer, unordered_map<string, char> const& rules) {
    string resultant_polymer = original_polymer;
    for (long s = 1; s <= step; s++) {
        resultant_polymer = *(polymerize(resultant_polymer, rules));
    }
    unordered_map<char, long> counts = *countCharOccurrence(resultant_polymer);
    auto result = minmax_element(counts.begin(), counts.end(), 
        [](auto a, auto b) {
            if (a.second < b.second) {
                return true;
            }
            return false;
        }
    );
    return (result.second->second - result.first->second);    
}

// Simulation by counting unique 2-char substrings
long getSol2(long step, string const& original_polymer, unordered_map<string, char> const& rules) {
    unordered_map<string, long> pair_freqs;
    unordered_map<char, long> char_freqs;
    // initialize
    for_each(rules.begin(), rules.end(), 
        [&](auto kvpair) {
            pair_freqs[kvpair.first] = 0;
            char_freqs[kvpair.second] = 0;
        }
    );
    for (long i = 1; i < original_polymer.size(); i++) {
        string selection = {original_polymer[i - 1], original_polymer[i]};
        // pairs
        pair_freqs[selection] ++;

        // chars
        char_freqs[selection[0]] ++;
        char_freqs[selection[1]] ++;
    }
    
    for (long s = 1; s <= step; s++) {
        unordered_map<string, long> pair_freqs_next_step = pair_freqs;
        unordered_map<char, long> char_freqs_next_step = char_freqs;
        for_each(pair_freqs.begin(), pair_freqs.end(), 
            [&](auto const& kvpair) { // e.g., AB -> C, hence AB -> ACB containing AC, CB
                string lhs = kvpair.first;
                long conversion_count = kvpair.second;
                char rhs = rules.at(lhs);
                char_freqs_next_step[rhs] += conversion_count; // Extended size for each AB to ACB

                string conversion_1 = {lhs[0], rhs};
                string conversion_2 = {rhs, lhs[1]};
                pair_freqs_next_step[lhs] -= conversion_count;
                pair_freqs_next_step[conversion_1] += conversion_count;
                pair_freqs_next_step[conversion_2] += conversion_count;
            }
        );
        pair_freqs = pair_freqs_next_step;
        char_freqs = char_freqs_next_step;
    }

    auto minmax_pair = minmax_element(char_freqs.begin(), char_freqs.end(), 
        [](auto a, auto b) {
            if (a.second == 0) {
                return false; // No 0 counts
            }
            if (a.second < b.second) {
                return true;
            }
            return false;
        }
    );
    return (minmax_pair.second->second - minmax_pair.first->second);
}

// assumes empty string and map
void parser(string const& filepath, string& polymer, unordered_map<string, char>& rules) {
    ifstream inputf (filepath);
    getline (inputf, polymer);

    string nextrule;
    while (inputf.peek() != EOF) {
        getline(inputf, nextrule);
        if (nextrule == "" || nextrule == "\n") { continue; } // for passing line break between inputs
        rules[nextrule.substr(0, 2)] = *(nextrule.end() - 1);
    }
}

void test() {
    string polymer_in;
    unordered_map<string, char> rules;
    parser("../resource/q14/input_test", polymer_in, rules);
    // parser
    assert(rules.size() == 16);
    assert(polymer_in.size() == 4);

    // polymerize
    assert(*(polymerize(polymer_in, rules)) == "NCNBCHB");
    
    // getSol1
    assert(getSol1(10, polymer_in, rules) == 1588);

    // getSol2
    assert(getSol2(10, polymer_in, rules) == 1588);
    assert(getSol2(40, polymer_in, rules) == 2188189693529);
}

int main() {
    test();
    string polymer_in;
    unordered_map<string, char> rules;
    parser("../resource/q14/input", polymer_in, rules);

    // 1.
    cout << "Part 1: " << getSol1(10, polymer_in, rules) << endl;

    // 2.
    cout << "Part 2: " << getSol2(40, polymer_in, rules) << endl;

    return 0;
}