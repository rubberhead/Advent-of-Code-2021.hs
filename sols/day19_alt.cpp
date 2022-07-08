#include <array>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <tuple>
#include <optional>
#include <memory>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
using namespace std;
// try multithreading in this one, use #include <thread>

const int DIMENSIONS = 3;
typedef array<int64_t, DIMENSIONS> Vector_3d;
typedef array<Vector_3d, DIMENSIONS> Matrix_3d;

unique_ptr<Vector_3d> mult(const Vector_3d& orig, const Matrix_3d& transformation) {
    unique_ptr<Vector_3d> result_ptr = make_unique<Vector_3d>(orig);
    Vector_3d& result_ref = *result_ptr;
    for (int i = 0; i < DIMENSIONS; i++) {
        const Vector_3d& row = transformation[i];
        result_ref[i] = orig[0] * row[0] + orig[1] * row[1] + orig[2] * row[2];
    }
    return (move(result_ptr));
}

Vector_3d operator + (const Vector_3d& lhs, const Vector_3d& rhs) {
    return (Vector_3d{lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]});
}

Vector_3d operator - (const Vector_3d& lhs, const Vector_3d& rhs) {
    return (Vector_3d{lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]});
}

const array<Matrix_3d, 24> ORIENTATIONS = { // TODO: ERRONEOUS ROTATIONS along facing axis!
    // facing +x -----------------------------------------------
    Matrix_3d{ // facing +x, R+y U+z -> rotate 4 times around x
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}, 
        Vector_3d{0, 0, 1}
    }, 
    Matrix_3d{ // facing +x, R-z U+y
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0,-1}, 
        Vector_3d{0, 1, 0}
    },
    Matrix_3d{ // facing +x, R-y U-z
        Vector_3d{1, 0, 0}, 
        Vector_3d{0,-1, 0}, 
        Vector_3d{0, 0,-1}
    },
    Matrix_3d{ // facing +x, R+z U-y
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0, 1}, 
        Vector_3d{0,-1, 0}
    },

    // facing +y ------------------------------------------------
    Matrix_3d{ // facing +y, +x+z -> rotate 4 times around y
        Vector_3d{0, 1, 0}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0, 1}
    }, 
    Matrix_3d{ // facing +y, -x+z
        Vector_3d{ 0, 1, 0}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0, 0, 1}
    }, 
    Matrix_3d{ // facing +y, +x-z
        Vector_3d{0, 1, 0}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0,-1}
    },
    Matrix_3d{ // facing +y, -x-z
        Vector_3d{ 0, 1, 0}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0, 0,-1}
    },

    // facing +z -----------------------------------------------
    Matrix_3d{ // facing +z, +x+y -> rotate 4 times around z
        Vector_3d{0, 0, 1}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}
    },
    Matrix_3d{ // facing +z, -x+y
        Vector_3d{ 0, 0, 1}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0, 1, 0}
    },
    Matrix_3d{ // facing +z, +x-y
        Vector_3d{0, 0, 1}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0,-1, 0}
    },
    Matrix_3d{ // facing +z, -x-y
        Vector_3d{ 0, 0, 1}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0,-1, 0}
    },

    // facing -x -----------------------------------------------
    Matrix_3d{ // facing -x, +y+z -> rotate 4 times around x
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0, 1, 0}, 
        Vector_3d{ 0, 0, 1}
    }, 
    Matrix_3d{ // facing -x, -y+z
        Vector_3d{-1, 0, 0}, 
        Vector_3d{0, -1, 0}, 
        Vector_3d{0,  0, 1}
    },
    Matrix_3d{ // facing -x, +y-z
        Vector_3d{-1, 0, 0}, 
        Vector_3d{0,  1, 0}, 
        Vector_3d{0,  0,-1}
    },
    Matrix_3d{ // facing -x, -y-z
        Vector_3d{-1, 0, 0}, 
        Vector_3d{0, -1, 0}, 
        Vector_3d{0,  0,-1}
    },

    // facing -y ----------------------------------------------
    Matrix_3d{ // facing -y, +x+z -> rotate 4 times around y
        Vector_3d{0,-1, 0}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0, 1}
    }, 
    Matrix_3d{ // facing -y, -x+z
        Vector_3d{ 0,-1, 0}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0, 0, 1}
    }, 
    Matrix_3d{ // facing -y, +x-z
        Vector_3d{0,-1, 0}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0,-1}
    },
    Matrix_3d{ // facing -y, -x-z
        Vector_3d{ 0,-1, 0}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0, 0,-1}
    },

    // facing -z ---------------------------------------------
    Matrix_3d{ // facing -z, +x+y -> rotate 4 times around z
        Vector_3d{0, 0,-1}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}
    }, 
    Matrix_3d{ // facing -z, -x+y
        Vector_3d{ 0, 0,-1}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0, 1, 0}
    },
    Matrix_3d{ // facing -z, +x-y
        Vector_3d{0, 0,-1}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0,-1, 0}
    },
    Matrix_3d{ // facing -z, -x-y
        Vector_3d{ 0, 0,-1}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{ 0,-1, 0}
    }
};

struct Scanner {
    static const int64_t THRESHOLD = 12;
    vector<Vector_3d> beacon_detections; // coordinates relative to *this* beacon at initialization, will be oriented in part 1.

    // For normalization
    bool isNormalized = false;
    bool isOriented = false;
    shared_ptr<Scanner> closest = nullptr;
    vector<Vector_3d> absolute_beacon_detections;
    Vector_3d delta_from_closest;

    bool isTrueCenter = false;

    Scanner() = default;
    Scanner(Scanner&&) = default;

    friend istream& operator >> (istream&, Scanner&);

    // return (ptr to) corresponding absolute distance vectors to *this* scanner that *first* satisfies overlapping threshold 
    // as well as the necessary transformation.
    // If unsuccessful, returns tuple w/ <0> as nullptr
    vector<Vector_3d> getFirstOverlappingTransformation(Scanner& other) {
        vector<vector<Vector_3d>> other_detection_variations = *other._getCoordinateVariations();

        for (int i = 0; i < ORIENTATIONS.size(); i++) {
            vector<Vector_3d>& detections = other_detection_variations[i];
            map<Vector_3d, int64_t> kvs = *(this->_getPossibleVecDiffs(detections));
            int64_t curr_orientation_max = max_element(kvs.begin(), kvs.end(), [](auto kv1, auto kv2){ return (kv1.second < kv2.second); })->second;
            cout << "In this orientation, curr_orientation_max = " << curr_orientation_max;
            cout << " out of " << kvs.size() << " values." << endl;
            for (auto kv : kvs) {
                if (kv.second >= Scanner::THRESHOLD) { // found first likely? distance that satisfies THRESHOLD
                    cout << kv.second << endl;
                    // coordinate deltas (i.e., relative position of other from this sensor)
                    other.closest = shared_ptr<Scanner>(this);
                    other.delta_from_closest = kv.first;
                    // Absolute distance vectors
                    other.beacon_detections = vector<Vector_3d>(detections); // oriented correctly to closest
                    other.isOriented = true;
                    other.absolute_beacon_detections = other._traceNormalizeToCenter(detections);
                    other.isNormalized = true;

                    // trace-normalization until true center
                    return (other.absolute_beacon_detections);
                }
            }
        }

        // otherwise, none of the transformations satisfy overlapping threshold.
        return {};
    }

private:
    // Normalize correctly-oriented coordinates to dist relative to true center (scanner 0).
    vector<Vector_3d> _traceNormalizeToCenter(vector<Vector_3d>& oriented) {
        if (this->isTrueCenter) {
            return oriented;
        }
        if (!isOriented) {
            throw (invalid_argument("Caller Scanner object not oriented yet."));
        }
        for_each(oriented.begin(), oriented.end(), 
            [&](Vector_3d& vec) {
                vec = vec + this->delta_from_closest;
            }
        );
        return (closest->_traceNormalizeToCenter(oriented));
    }

    // Return unique_ptr to a vector of all possible orientations to the original relative coordinates as 
    // detected by this Scanner.
    unique_ptr<vector<vector<Vector_3d>>> _getCoordinateVariations() {
        vector<vector<Vector_3d>> results;
        results.push_back(this->beacon_detections);
        int idx = 0;
        for_each(ORIENTATIONS.begin() + 1, ORIENTATIONS.end(), // begin() -> Identity matrix
            [&](const Matrix_3d& trans) {
                results.push_back({});
                idx++;
                for (const Vector_3d& vec : beacon_detections) {
                    results[idx].push_back(*mult(vec, trans));
                }
            }
        );
        return (make_unique<vector<vector<Vector_3d>>>(move(results)));
    }

    unique_ptr<map<Vector_3d, int64_t>> _getPossibleVecDiffs(const vector<Vector_3d>& b1_detections) {
        // ... relative to this beacon's orientation, 
        // no guarantee b1 at same orientation as this beacon! 
        map<Vector_3d, int64_t> results;
        for (const Vector_3d& b1_p1 : b1_detections) {
            if (b1_p1 == Vector_3d{-660,-479,-426}) {
                cout << "Hey what's happening?" << endl;
            }
            for (const Vector_3d& b0_p0 : this->beacon_detections) {
                results[b0_p0 - b1_p1]++;
            }
        }
        return (make_unique<map<Vector_3d, int64_t>>(move(results)));
    }
};

// read "x,y,z" rows until line break, return read istream
// input in should be aligned AT the first x,y,z row
istream& operator >> (istream& in, Scanner& scanner) {
    string nextline;
    while (in.peek() != EOF) {
        getline(in, nextline);
        if (nextline == "") { break; } // in now @ new scanner line
        // otherwise, read x
        size_t first_comma_idx = nextline.find_first_of(',', 0);
        int64_t x = static_cast<int64_t>(stoi(nextline.substr(0, first_comma_idx)));
        // then y
        size_t second_comma_idx = nextline.find_first_of(',', first_comma_idx + 1);
        int64_t y = static_cast<int64_t>(stoi(nextline.substr(first_comma_idx + 1, second_comma_idx)));
        // finally z
        int64_t z = static_cast<int64_t>(stoi(nextline.substr(second_comma_idx + 1)));

        // insert to scanner
        scanner.beacon_detections.push_back(Vector_3d{x, y, z});
    }
    return in; // @ new scanner line or EOF
}

vector<shared_ptr<Scanner>> parser(const string& filepath) {
    ifstream inputf (filepath);
    string nextline;
    vector<shared_ptr<Scanner>> results;
    while (inputf.peek() != EOF) {
        getline(inputf, nextline);
        if (nextline.substr(0, 3) == "---") { // new scanner
            Scanner next_scanner;
            inputf >> next_scanner;
            results.push_back(make_shared<Scanner>(move(next_scanner))); // intend to move xvalue if this is incorrect
            continue;
        } else { // error: 
            throw (runtime_error("Invalid istream alignment: x,y,z tuple not read by >> operator."));
        }
    }
    results[0]->isTrueCenter = true;
    return results;
}

size_t getSol1(vector<shared_ptr<Scanner>>& scanner_ptrs) {
    // Add beacons detected by scanner 0
    Scanner& center = *scanner_ptrs[0];
    set<Vector_3d> distinct_pts;
    vector<Scanner*> fixed;
    auto lambdaInsert = [&](Vector_3d coordinate) -> void { // pass-by-value or by-reference?
        distinct_pts.insert(coordinate);
    };
    for_each(center.beacon_detections.begin(), 
             center.beacon_detections.end(), 
             lambdaInsert);
    fixed.push_back(scanner_ptrs[0].get());

    // Iteratively produce overlap relationships between different scanners other than scanner 0
    queue<Scanner*> work;
    for_each(scanner_ptrs.begin() + 1, scanner_ptrs.end(), 
        [&](shared_ptr<Scanner>& ptr) { work.push(ptr.get()); });

    while (!work.empty()) {
        Scanner* si_ptr = work.front();
        work.pop();
        for (Scanner* sc_ptr : fixed) {
            vector<Vector_3d> new_coords = sc_ptr->getFirstOverlappingTransformation(*si_ptr);
            if (!new_coords.empty()) {
                fixed.push_back(si_ptr);
                for_each(new_coords.begin(), new_coords.end(), lambdaInsert);
            }
        }
        if (fixed.back() != si_ptr) {
            work.push(si_ptr);
        }
    }

    return (distinct_pts.size());
}

void test() {
    // vector-matrix multiplication
    Vector_3d test_vec {1, 2, 3};
    Matrix_3d test_mat {
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}, 
        Vector_3d{0, 0, 1}
    };
    assert(*mult(test_vec, test_mat) == test_vec);
    test_mat = Matrix_3d{
        Vector_3d{-1, 0, 0}, 
        Vector_3d{0, 0, 1}, 
        Vector_3d{0, -1, 0}
    };
    Vector_3d result = *mult(test_vec, test_mat);
    assert(result == (Vector_3d{-1, 3, -2}));

    // parser, >>
    auto parsed = parser("../resource/q19/input");
    assert(parsed.size() == 30);
    
    // Scanner functionalities: getFirstOverlappingTransformation
    parsed = parser("../resource/q19/input_test");
    assert(parsed.size() == 5);
    Scanner& s0 = *parsed[0];
    Scanner& s1 = *parsed[1];
    Scanner& s4 = *parsed[4];
    size_t idx_4 = distance(s4.beacon_detections.begin(), 
                            find(s4.beacon_detections.begin(), s4.beacon_detections.end(), Vector_3d{479, 426, -660}));
    vector<Vector_3d> transformed = s0.getFirstOverlappingTransformation(s1);
    vector<Vector_3d> transformed_on_transformed = s1.getFirstOverlappingTransformation(s4);
    assert(!transformed.empty());
    assert(!transformed_on_transformed.empty());

    // getSol1
    size_t beacon_count_test = getSol1(parsed);
    assert(beacon_count_test == 79);
}

int main() {
    test();
    return 0;
}