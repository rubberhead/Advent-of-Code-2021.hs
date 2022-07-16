#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <algorithm>
#include <memory>
#include <bitset>
using namespace std;

struct Image {
    deque<deque<bool>> data; // include 1px border after >> call
    
    Image() = default;
    Image(Image&&) = default;

    friend istream& operator >> (istream&, Image&);
    friend ostream& operator << (ostream&, Image&);
};

// refer to model input, assume is aligned at first character of image input
istream& operator >> (istream& is, Image& new_img) {
    string nextline;
    while (is.peek() != EOF) {
        getline(is, nextline);
        new_img.data.push_back({});
        for (char& c : nextline) {
            bool next = (c == '#') ? 1 : 0;
            new_img.data.rbegin()->push_back(next);
        }
    }
    // after all lines read, add border 0s
    for_each(new_img.data.begin(), new_img.data.end(), // columns
        [](deque<bool>& row) {
            row.push_front(0);
            row.push_back(0);
        }
    );
    size_t row_len = new_img.data.begin()->size();
    // then, add 2 rows of 0s at front and back
    new_img.data.push_back(deque<bool>(row_len, 0));
    new_img.data.push_front(deque<bool>(row_len, 0));
    
    // and we are done!
    return is;
}

ostream& operator << (ostream& os, Image& curr_img) {
    for_each(curr_img.data.begin() + 1, curr_img.data.end() - 1, 
        [&](deque<bool>& row) {
            for_each(row.begin() + 1, row.end() - 1, 
                [&](bool& pix) {
                    char out_c = (pix == 1) ? '#' : '.';
                    os << out_c;
                }
            );
            os << endl;
        }
    ); // suppress border output
    return os;
}

// Helper function
uint64_t readIdxFromPix(Image const& curr_img, int i, int j) {
    bitset<9> result;
    int r = i - 1;
    int c = j - 1;
    size_t idx = 8;
    for (int r = i - 1; r <= i + 1; r++) {
        for (int c = j - 1; c <= j + 1; c++) {
            if (r < 0 || r >= curr_img.data.size() || c < 0 || c >= curr_img.data[0].size()) { // out of range (incl. border)
                result[idx] = false;
            } else { 
                result[idx] = curr_img.data[r][c];
            }
            idx--;
        }
    }
    return result.to_ullong();
}

unique_ptr<Image> enhance(string const& algo, Image const& curr_img) {
    // raw img: not cropped / extended compared to curr_img size
    Image new_img;
    for (size_t i = 0; i < curr_img.data.size(); i++) {
        new_img.data.push_back({});
        for (size_t j = 0; j < curr_img.data[i].size(); j++) {
            uint64_t idx = readIdxFromPix(curr_img, i, j);
            bool pix = (algo[idx] == '#') ? 1 : 0;
            new_img.data.rbegin()->push_back(pix);
        }
    }

    // Cropping ----------------------------------------------------------------------------------------------
    size_t last_col = new_img.data[0].size() - 1;
    // Crop front col until first non-all-zero
    while (all_of(new_img.data.begin(), new_img.data.end(), [](deque<bool>& row){ return (row[0] == 0); })) { 
        // while top col all-zero, O(row-size) time
        for_each(new_img.data.begin(), new_img.data.end(), // O(row-size) time
            [](deque<bool>& row) {
                row.pop_front(); // pop first col
            }
        );
    }
    // likewise, crop last col until first non-all-zero
    while (all_of(new_img.data.begin(), new_img.data.end(), [](deque<bool>& row){ return (row[row.size() - 1] == 0); })) { 
        // while end col all-zero, O(row-size) time
        for_each(new_img.data.begin(), new_img.data.end(), // O(row-size) time
            [](deque<bool>& row) {
                row.pop_back(); // pop first col
            }
        );
    }
    
    // Crop front row until first non-all-zero
    while (all_of(new_img.data.begin()->begin(), new_img.data.begin()->end(), [](bool& pix){ return (pix == 0); })) {
        // while top row all-zero, O(col-size) time
        new_img.data.pop_front(); // O(1) time?
    }
    // likewise, crop last row until first non-all-zero
    while (all_of(new_img.data.rbegin()->begin(), new_img.data.rbegin()->end(), [](bool& pix){ return (pix == 0); })) {
        // while end row all-zero, O(col-size) time
        new_img.data.pop_back(); // O(1) time?
    }
    // ------------------------------------------------------------------------------------------------------

    // Extension (for invariance w/ >>)
    for_each(new_img.data.begin(), new_img.data.end(), // columns
        [](deque<bool>& row) {
            row.push_front(0);
            row.push_back(0);
        }
    );
    size_t row_len = new_img.data[0].size();
    new_img.data.push_back(deque<bool>(row_len, 0)); // rows
    new_img.data.push_front(deque<bool>(row_len, 0));
    
    // done! 
    return (make_unique<Image>(move(new_img)));
}

void test() {
    // >>, << for Image objects
    string test_simple_img_str = "#..#.\n#....\n##..#\n..#..\n..###";
    stringstream tsi_st (test_simple_img_str);
    Image test_simple_img;
    tsi_st >> test_simple_img;
    stringstream tsi_out;
    tsi_out << test_simple_img;
    string tsi_out_str = tsi_out.str();
    assert(tsi_out_str.substr(0, tsi_out_str.size() - 1) == test_simple_img_str);

    // Enhance Image object
    cout << test_simple_img << endl;
    string test_simple_img_algo = "..#.#..#####.#.#.#.###.##.....###.##.#..###.####..#####..#....#..#..##..###..######.###...####..#..#####..##..#.#####...##.#.#..#.##..#.#......#.###.######.###.####...#.##.##..#..#..#####.....#.#....###..#.##......#.....#..#..#..##..#...##.######.####.####.#.#...#.......#..#.#.#...####.##.#......#..#...##.#.##..#...##.#.##..###.#......#.#.......#.#.#.####.###.##...#.....####.#..#..#.##.#....##..#.####....##...##..#...#......#.#.......#.......##..####..#...#.#.#...##..#.#..###..#####........#..####......#..#";
    unique_ptr<Image> tsi_enhanced_ptr = enhance(test_simple_img_algo, test_simple_img);
    cout << *tsi_enhanced_ptr << endl;
    tsi_enhanced_ptr = enhance(test_simple_img_algo, *tsi_enhanced_ptr);
    cout << *tsi_enhanced_ptr << endl;   

}

int main() {
    test();
    return 0;
}