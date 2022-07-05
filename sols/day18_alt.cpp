// Basically same as day18.cpp but rewritten in C-style pointers. Couldn't handle them
// Simon Toth's blog has been a huge help - I wasn't quite understanding of how to write custom data structures in C++ 
// so his code was of remarkable referential quality.
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <tuple>
#include <memory>
#include <optional>
#include <algorithm>
#include <numeric>
#include <cassert>
using namespace std;

struct SNTreeNode {
    SNTreeNode* left = nullptr;
    SNTreeNode* right = nullptr;
    SNTreeNode* parent = nullptr;
    optional<int> data;

    SNTreeNode() = default;
    SNTreeNode(SNTreeNode &&) = default; // move
    SNTreeNode& operator = (SNTreeNode&&) = default;

    SNTreeNode(SNTreeNode* parent_ptr) {
        parent = parent_ptr;
    }

    SNTreeNode(int value, SNTreeNode* parent_ptr) {
        data = value;
        parent = parent_ptr;
    }

    SNTreeNode(SNTreeNode const& other) { // copy
        if (other.isTerminal()) {
            return;
        }
        left = other.left != nullptr ? new SNTreeNode(*other.left) : nullptr;
        right = other.right != nullptr ? new SNTreeNode(*other.right) : nullptr;
        parent = other.parent != nullptr ? new SNTreeNode(*other.parent) : nullptr;
    }

    bool isTerminal() const {
        return (left == nullptr && right == nullptr);
    }

    bool isUninitialized() const {
        return (this->isTerminal() && parent == nullptr && !data.has_value());
    }

    operator int() const {
        if (this->isTerminal()) {
            return this->data.value_or(-1);
        }
        return 3 * static_cast<int>(*this->left) + 2 * static_cast<int>(*this->right);
    }
};

bool operator == (SNTreeNode& a, SNTreeNode& b) {
    return (a.parent == b.parent && // pointer equality
            a.left == b.left &&
            a.right == b.right &&
            a.data.value_or(-1) == b.data.value_or(-1)); // value equality
}

istream& operator >> (istream& is, SNTreeNode& curr_top) {
    char nextChar;
    if (!(is >> nextChar) || nextChar != '[') {
        throw (invalid_argument("Input stream invalid: should begin with ["));
    }
    curr_top.left = new SNTreeNode(&curr_top);
    if (is.peek() == '[') {
        // deepen on left
        is >> *(curr_top.left);
    } else {
        int leftValue;
        is >> leftValue;
        curr_top.left->data = leftValue;
    }

    if (!(is >> nextChar) || nextChar != ',') {
        throw (invalid_argument("Input stream invalid: should have , as delimiter between two nodes"));
    }

    curr_top.right = new SNTreeNode(&curr_top);
    if (is.peek() == '[') {
        // deepen on right
        is >> *(curr_top.right);
    } else {
        int rightValue;
        is >> rightValue;
        curr_top.right->data = rightValue;
    }

    if (!(is >> nextChar) || nextChar != ']') {
        throw (invalid_argument("Input stream invalid: should end with ]"));
    }

    return is;
}

ostream& operator << (ostream& os, SNTreeNode const& curr_top) {
    if (curr_top.isTerminal()) {
        os << curr_top.data.value_or(-1);
        return os;
    }
    os << "[" << *(curr_top.left) << "," << *(curr_top.right) << "]";
    return os;
}

SNTreeNode* findMostAdjacentOnL(SNTreeNode& curr_sn) {
    SNTreeNode* temp_parent = &curr_sn;
    while (*temp_parent->parent->left == *temp_parent) {
        temp_parent = temp_parent->parent;
        if (temp_parent->parent == nullptr) {
            return nullptr;
        }
    }
    temp_parent = temp_parent->parent;
    SNTreeNode* findFrom = temp_parent->left;
    while (!findFrom->isTerminal()) {
        findFrom = findFrom->right;
    }
    return (findFrom);
}

SNTreeNode* findMostAdjacentOnR(SNTreeNode& curr_sn) {
    SNTreeNode* temp_parent = &curr_sn;
    while (*temp_parent->parent->right == *temp_parent) {
        temp_parent = temp_parent->parent;
        if (temp_parent->parent == nullptr) {
            return nullptr;
        }
    } 
    temp_parent = temp_parent->parent;
    SNTreeNode* findFrom = temp_parent->right;
    while (!findFrom->isTerminal()) {
        findFrom = findFrom->left;
    }
    return (findFrom);
}

bool explode(SNTreeNode& curr_sn, int depth = 1) {
    bool flag = false;
    if (!curr_sn.left->isTerminal()) {
        flag = explode(*curr_sn.left, depth + 1);
    }
    if (!flag && !curr_sn.right->isTerminal()) {
        flag = explode(*curr_sn.right, depth + 1);
    }
    if (depth > 4) {
        SNTreeNode* curr_sn_parent = curr_sn.parent;
        if (curr_sn_parent->left == &curr_sn || curr_sn_parent->right == &curr_sn) {
            SNTreeNode* adj_left_rsn = findMostAdjacentOnL(curr_sn);
            if (adj_left_rsn != nullptr) {
                adj_left_rsn->data = adj_left_rsn->data.value() + curr_sn.left->data.value();
            }

            SNTreeNode* adj_right_rsn = findMostAdjacentOnR(curr_sn);
            if (adj_right_rsn != nullptr) {
                adj_right_rsn->data = adj_right_rsn->data.value() + curr_sn.right->data.value();
            }

            // wasn't able to fully comprehend smart pointers so done the unsafe way instead
            curr_sn.data = 0;
            delete (curr_sn.left);
            curr_sn.left = nullptr;
            delete (curr_sn.right);
            curr_sn.right = nullptr;
        } else {
            throw runtime_error("curr_sn is somehow not on left nor right of its parent");
        }
        flag = true;
        return flag;
    } else {
        return flag; // carry-over from prev. stack trace or false when condition mismatch
    }
}

bool split(SNTreeNode& curr_sn) {
    bool flag = false;
    if (!curr_sn.isTerminal()) {
        flag = split(*curr_sn.left);
        if (!flag) {
            flag = split(*curr_sn.right);
        }
    }
    
    if (curr_sn.data.value_or(-1) > 9) {
        int data = curr_sn.data.value();
        curr_sn.left = new SNTreeNode(data / 2, &curr_sn);
        curr_sn.right = new SNTreeNode((data + 1) / 2, &curr_sn);
        curr_sn.data = nullopt;
        flag = true;
        return flag;
    } else {
        return flag;
    }
}

void reduce(SNTreeNode& curr_sn) {
    bool flag = false;
    do {
        flag = explode(curr_sn, 1);
        if (!flag) {
            flag = split(curr_sn);
        }
    } while (flag);
}

SNTreeNode&& operator + (SNTreeNode& lhs, SNTreeNode& rhs) {
    if (lhs.parent != nullptr || rhs.parent != nullptr) {
        throw (invalid_argument("LHS/RHS of + operator need to not be subtrees"));
    }
    if (lhs.isUninitialized()) {
        return (move(rhs));
    }
    if (rhs.isUninitialized()) {
        return (move(lhs));
    }
    // otherwise, both are initialized
    SNTreeNode* new_parent = new SNTreeNode();
    lhs.parent = new_parent;
    new_parent->left = &lhs;
    rhs.parent = new_parent;
    new_parent->right = &rhs;
    reduce(*new_parent);
    return (move(*new_parent));
}

// EXTREMELY hack-y way of performing recursive plus actions
// likely very costly, mostly due to NOT utilizing the copy constructor but instead 
// utilizing chained calls to >> and << operators.
// why? -- I was not familiar with how to change the memory location pointed to by an 
// (lvalue) reference through rvalue returns. I must have tried pointers as well 
// but iirc they don't work as I intended too either. idk i'm just not good enough
SNTreeNode&& recursivePlus(SNTreeNode&& first_sn, istream& curr_in) {
    if (curr_in.peek() == EOF) {
        cout << "SUM: " << first_sn << endl;
        return(move(first_sn));
    }
    string nextline;
    getline(curr_in, nextline);
    stringstream nls (nextline);
    SNTreeNode rhs;
    nls >> rhs;
    stringstream result;
    result << (first_sn + rhs);
    SNTreeNode next;
    result >> next;
    return(recursivePlus(move(next), curr_in));
}

void test() {
    /*
    // struct io
    stringstream test_1 ("[1,1]");
    SNTreeNode test_sn_1;
    test_1 >> test_sn_1;
    cout << test_sn_1 << endl;

    SNTreeNode test_sn_2;
    stringstream test_2 ("[[[[[9,8],1],2],3],4]");
    test_2 >> test_sn_2;
    cout << test_sn_2 << endl;

    // explode
    SNTreeNode test_sn_3;
    stringstream test_3 ("[[[[[9,8],1],2],3],4]");
    test_3 >> test_sn_3;
    bool flag = explode(test_sn_3, 1);
    cout << test_sn_3 << endl;
    assert(flag == true);
    assert(explode(test_sn_3, 1) == false);

    SNTreeNode test_sn_4;
    stringstream test_4 ("[7,[6,[5,[4,[3,2]]]]]");
    test_4 >> test_sn_4;
    flag = explode(test_sn_4, 1);
    cout << test_sn_4 << endl;
    assert(flag == true);

    SNTreeNode test_sn_5;
    stringstream test_5 ("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]");
    test_5 >> test_sn_5;
    for (int i = 0; i < 2; i++) {
        flag = explode(test_sn_5, 1);
        cout << test_sn_5 << endl;
        assert(flag == true);
    }

    // split
    SNTreeNode test_sn_6;
    stringstream test_6 ("[[[[0,7],4],[15,[0,13]]],[1,1]]");
    test_6 >> test_sn_6;
    flag = split(test_sn_6);
    cout << test_sn_6 << endl;
    assert(flag == true);

    // reduce
    SNTreeNode test_sn_7;
    stringstream test_7 ("[[[[[4,3],4],4],[7,[[8,4],9]]],[1,1]]");
    test_7 >> test_sn_7;
    reduce(test_sn_7);
    cout << test_sn_7 << endl; */
    
    // plus
    SNTreeNode test_sn_lhs, test_sn_rhs;
    stringstream test_lhs ("[[[[9,7],[7,8]],[[8,8],[0,8]]],[[[8,8],[9,9]],[[0,7],6]]]");
    stringstream test_rhs ("[[[8,5],[[0,0],[4,9]]],[2,8]]");
    test_lhs >> test_sn_lhs;
    test_rhs >> test_sn_rhs;
    SNTreeNode result = test_sn_lhs + test_sn_rhs;
    cout << result << endl;

    /*
    // plus on uninitialized
    test_sn_lhs = SNTreeNode();
    test_sn_rhs = SNTreeNode();
    test_rhs = stringstream("[6,[9,8]]");
    test_rhs >> test_sn_rhs;
    result = test_sn_lhs + test_sn_rhs;
    cout << result << endl; */

    // recurrent plus from file input
    ifstream inputf ("../resource/q18/input_test");
    SNTreeNode sum = recursivePlus(move(SNTreeNode()), inputf);
    // magnitude
    cout << "has magnitude: " << static_cast<int>(sum) << endl;

}

int main() {
    test();

    // 1.
    ifstream inputf ("../resource/q18/input");
    cout << "Part 1: " << endl;
    SNTreeNode sum = recursivePlus(move(SNTreeNode()), inputf);
    cout << "has magnitude: " << static_cast<int>(sum) << endl;

    // 2.
    cout << "Part 2: " << endl;
    int max = 0;
    inputf = ifstream("../resource/q18/input");
    // ... 



    return 0;
}
