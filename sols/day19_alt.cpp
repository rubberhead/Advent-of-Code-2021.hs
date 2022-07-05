#include <array>
#include <vector>
#include <set>
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

unique_ptr<vector<Matrix_3d>> flipXHeading (const vector<Matrix_3d>& matrices) {
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

unique_ptr<vector<Matrix_3d>> flipYHeading (const vector<Matrix_3d>& matrices) {
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

unique_ptr<vector<Matrix_3d>> flipZHeading (const vector<Matrix_3d>& matrices) {
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

unique_ptr<set<Matrix_3d>> getUniqueOrientationMatrices() { // only 19, gotta do more variety or make the table yourself
    auto results_ptr = make_unique<set<Matrix_3d>>(); // invert inverts all 3, x/y/z inverts just 1, try inverting all 1s and 2s in addition
    // original rotation matrices
    for_each(ROTATION_MATRICES_AROUND_X.begin(), 
             ROTATION_MATRICES_AROUND_X.end(), 
             [&](const Matrix_3d& mat) {
                results_ptr->insert(mat);
            });
    for_each(ROTATION_MATRICES_AROUND_Y.begin(), 
             ROTATION_MATRICES_AROUND_Y.end(), 
             [&](const Matrix_3d& mat) {
                results_ptr->insert(mat);
            });
    for_each(ROTATION_MATRICES_AROUND_Z.begin(), 
             ROTATION_MATRICES_AROUND_Z.end(), 
             [&](const Matrix_3d& mat) {
                results_ptr->insert(mat);
            });

    // flipped rotation matrices
    auto flipped_x = *flipXHeading(ROTATION_MATRICES_AROUND_X);
    for_each(flipped_x.begin(), flipped_x.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });
    auto flipped_y = *flipYHeading(ROTATION_MATRICES_AROUND_Y);
    for_each(flipped_y.begin(), flipped_y.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });
    auto flipped_z = *flipZHeading(ROTATION_MATRICES_AROUND_Z);
    for_each(flipped_z.begin(), flipped_z.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });

    // inverted rotation matrices
    auto inverted_x = *invert(ROTATION_MATRICES_AROUND_X);
    for_each(inverted_x.begin(), inverted_x.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });
    auto inverted_y = *invert(ROTATION_MATRICES_AROUND_Y);
    for_each(inverted_y.begin(), inverted_y.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });
    auto inverted_z = *invert(ROTATION_MATRICES_AROUND_Z);
    for_each(inverted_z.begin(), inverted_z.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });

    // inverted & flipped rotation matrices
    auto inverted_flipped_x = *invert(vector<Matrix_3d>(forward<const vector<Matrix_3d>&>(flipped_x)));
    for_each(inverted_flipped_x.begin(), inverted_flipped_x.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });
    auto inverted_flipped_y = *invert(vector<Matrix_3d>(forward<const vector<Matrix_3d>&>(flipped_y)));
    for_each(inverted_flipped_y.begin(), inverted_flipped_y.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });    
    auto inverted_flipped_z = *invert(vector<Matrix_3d>(forward<const vector<Matrix_3d>&>(flipped_z)));
    for_each(inverted_flipped_z.begin(), inverted_flipped_z.end(), [&](Matrix_3d& mat) {
        results_ptr->insert(mat);
    });

    return results_ptr;
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
    auto flipped = *flipXHeading(ROTATION_MATRICES_AROUND_X);
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

    // get all orientation transformation matrices
    auto all_transformations = *getUniqueOrientationMatrices();
    assert(all_transformations.size() == 24);
}

int main() {
    test();
    return 0;
}