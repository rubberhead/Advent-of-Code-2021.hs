#include <array>
#include <vector>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <cassert>
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

const vector<Matrix_3d> ROTATION_MATRICES_AROUND_X = {
    Matrix_3d{ // R_x_0
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}, 
        Vector_3d{0, 0, 1}
    }, 
    Matrix_3d{ // R_x_90
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0,-1}, 
        Vector_3d{0, 1, 0}
    }, 
    Matrix_3d{ // R_x_180
        Vector_3d{1, 0, 0}, 
        Vector_3d{0,-1, 0}, 
        Vector_3d{0, 0,-1}
    }, 
    Matrix_3d{ // R_x_270
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0, 1}, 
        Vector_3d{0,-1, 0}
    }
};

const vector<Matrix_3d> ROTATION_MATRICES_AROUND_Y = {
    Matrix_3d{ // R_y_0
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}, 
        Vector_3d{0, 0, 1}
    }, 
    Matrix_3d{ // R_y_90
        Vector_3d{0,  0, 1}, 
        Vector_3d{0,  1, 0}, 
        Vector_3d{-1, 0, 0}
    }, 
    Matrix_3d{ // R_y_180
        Vector_3d{-1, 0, 0}, 
        Vector_3d{0,  1, 0}, 
        Vector_3d{0,  0,-1}
    }, 
    Matrix_3d{ // R_y_270
        Vector_3d{0, 0,-1}, 
        Vector_3d{0, 1, 0}, 
        Vector_3d{1, 0, 0}
    }
};

const vector<Matrix_3d> ROTATION_MATRICES_AROUND_Z = {
    Matrix_3d{ // R_z_0
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 1, 0}, 
        Vector_3d{0, 0, 1}
    }, 
    Matrix_3d{ // R_z_90
        Vector_3d{0,-1, 0}, 
        Vector_3d{1, 0, 0}, 
        Vector_3d{0, 0, 1}
    }, 
    Matrix_3d{ // R_z_180
        Vector_3d{-1, 0, 0}, 
        Vector_3d{0, -1, 0}, 
        Vector_3d{0,  0, 1}
    }, 
    Matrix_3d{ // R_z_270
        Vector_3d{0,  1, 0}, 
        Vector_3d{-1, 0, 0}, 
        Vector_3d{0,  0, 1}
    }
};

unique_ptr<vector<Matrix_3d>> filpXHeading (const vector<Matrix_3d>& matrices) {
    auto results_ptr = make_unique<vector<Matrix_3d>>(matrices);
    for_each(results_ptr->begin(), results_ptr->end(), 
        [](Matrix_3d& this_mat) {
            for (int i = 0; i < DIMENSIONS; i++) {
                for (int j = 0; j < DIMENSIONS; j++) {
                    if (j == 0 && this_mat[i][j] != 0) {
                        this_mat[i][j] *= -1;
                    }
                }
            }
        }
    );
    return results_ptr;
}

unique_ptr<vector<Matrix_3d>> filpYHeading (const vector<Matrix_3d>& matrices) {
    auto results_ptr = make_unique<vector<Matrix_3d>>(matrices);
    for_each(results_ptr->begin(), results_ptr->end(), 
        [](Matrix_3d& this_mat) {
            for (int i = 0; i < DIMENSIONS; i++) {
                for (int j = 0; j < DIMENSIONS; j++) {
                    if (j == 1 && this_mat[i][j] != 0) {
                        this_mat[i][j] *= -1;
                    }
                }
            }
        }
    );
    return results_ptr;
}

unique_ptr<vector<Matrix_3d>> filpZHeading (const vector<Matrix_3d>& matrices) {
    auto results_ptr = make_unique<vector<Matrix_3d>>(matrices);
    for_each(results_ptr->begin(), results_ptr->end(), 
        [](Matrix_3d& this_mat) {
            for (int i = 0; i < DIMENSIONS; i++) {
                for (int j = 0; j < DIMENSIONS; j++) {
                    if (j == 2 && this_mat[i][j] != 0) {
                        this_mat[i][j] *= -1;
                    }
                }
            }
        }
    );
    return results_ptr;
}

unique_ptr<vector<Matrix_3d>> invert (const vector<Matrix_3d>& matrices) {
    auto results_ptr = make_unique<vector<Matrix_3d>>(matrices);
    for_each(results_ptr->begin(), results_ptr->end(), 
        [](Matrix_3d& this_mat) {
            for (int i = 0; i < DIMENSIONS; i++) {
                for (int j = 0; j < DIMENSIONS; j++) {
                    this_mat[i][j] *= -1;
                }
            }
        }
    );
    return results_ptr;
}

unique_ptr<unordered_set<Matrix_3d>> getUniqueOrientationMatrices() {
    auto results_ptr = make_unique<unordered_set<Matrix_3d>>();
    // original rotation matrices

    // flipped rotation matrices

    // inverted rotation matrices

    // inverted & flipped rotation matrices
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

    // flip axis
    auto flipped = *filpXHeading(ROTATION_MATRICES_AROUND_X);
    assert(flipped[0] 
            == (Matrix_3d{
                Vector_3d{-1, 0, 0}, 
                Vector_3d{0, 1, 0}, 
                Vector_3d{0, 0, 1}
                })
    ); // TODO: add test for flip on Y, Z axes, expand test for all entries.

    // Invert all entries
    auto inverted = *invert(ROTATION_MATRICES_AROUND_X);
    assert(inverted[0]
            == (Matrix_3d{
                Vector_3d{-1, 0, 0}, 
                Vector_3d{0, -1, 0}, 
                Vector_3d{0, 0, -1}
            })
    ); // TODO: expand test for all entries.
}

int main() {
    test();
    return 0;
}