#include <iostream>
#include <fstream>
#include <array> // non-mutable data structure
#include <numerics>
#include <algorithm>
using namespace std;

struct LinalgVec {
	array<int64_t> arr_3d = {0, 0, 0};

	template<typename T, template<typename, typename> class Container>
	LinalgVec(Container<T, std::allocator<T>> const& init_arr) {
		if (init_arr.size() < 3) {
			throw (invalid_argument("init_arr should be at least of size 3."));
		}
		arr_3d[0] = static_cast<int64_t>(init_arr[0]);
		arr_3d[1] = static_cast<int64_t>(init_arr[1]);
		arr_3d[2] = static_cast<int64_t>(init_arr[2]);
	}
	
	LinalgVec&& cross(Container<LinalgVec, std::allocator<LinalgVec>> linalg_mat) {
		
	}
	
	// overload [] operator
};

