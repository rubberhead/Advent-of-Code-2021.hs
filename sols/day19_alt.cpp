#include <array>
#include <vector>
#include <memory>
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

Vector_3d&& operator + (Vector_3d& lhs, Vector_3d& rhs) {
    return (Vector_3d{lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]});
}

Vector_3d&& operator - (Vector_3d& lhs, Vector_3d& rhs) {
    return (Vector_3d{lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]});
}

const array<Matrix_3d, 24> ORIENTATIONS = {
    // facing +x -----------------------------------------------
    Matrix_3d{ // facing +x, +y+z -> rotate 4 times around x
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}, 
        Vector_3d{0, 0, 1}
    }, 
    Matrix_3d{ // facing +x, -y+z
        Vector_3d{1, 0, 0}, 
        Vector_3d{0,-1, 0}, 
        Vector_3d{0, 0, 1}
    },
    Matrix_3d{ // facing +x, +y-z
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}, 
        Vector_3d{0, 0,-1}
    },
    Matrix_3d{ // facing +x, -y-z
        Vector_3d{1, 0, 0}, 
        Vector_3d{0,-1, 0}, 
        Vector_3d{0, 0,-1}
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
    vector<Vector_3d> beacon_detections; // coordinates relative to *this* beacon

    Scanner() = default;
    Scanner(Scanner&&) = default;

    friend istream& operator >> (istream&, Scanner&);

    vector<vector<Vector_3d>>&& getCoordinateVariations() {
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
        return (move(results));
    }

    
};

// read "x,y,z" rows until line break, return read istream
// at input in should be aligned BEFORE the first x,y,z row (e.g., at the "---" row)
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

vector<unique_ptr<Scanner>> parser(const string& filepath) {
    ifstream inputf (filepath);
    string nextline;
    vector<unique_ptr<Scanner>> results;
    while (inputf.peek() != EOF) {
        getline(inputf, nextline);
        if (nextline.substr(0, 3) == "---") { // new scanner
            Scanner next_scanner;
            inputf >> next_scanner;
            results.push_back(make_unique<Scanner>(move(next_scanner))); // move
            continue;
        } else { // error: 
            throw (runtime_error("Invalid istream alignment: x,y,z tuple not read by >> operator."));
        }
    }
    return results;
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
    
}

int main() {
    test();
    return 0;
}