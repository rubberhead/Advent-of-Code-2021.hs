#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <optional>
#include <memory>
#include <cassert>
using namespace std;

struct SnailNumber { // largely followed code structure from Simon Toth's blog
    optional<SnailNumber&&> leftSN;
    optional<SnailNumber&&> rightSN;
    optional<SnailNumber&&> parentSN;
    optional<int> regularData;

    SnailNumber() = default;
    SnailNumber(SnailNumber &&) = default;
    SnailNumber& operator = (SnailNumber &&) = default;

    SnailNumber(SnailNumber* _parent_ptr) {
        parentSN = shared_ptr<SnailNumber>(&*_parent_ptr); // awkward, yes
    }

    SnailNumber(int _regularData, unique_ptr<SnailNumber>& _parent_ptr) {
        regularData = _regularData;
        parentSN = shared_ptr<SnailNumber>(&*_parent_ptr);
    }

    SnailNumber(SnailNumber const& other){
        leftSN = make_shared<SnailNumber>(*other.leftSN);
        rightSN = make_shared<SnailNumber>(*other.rightSN);
        parentSN = make_shared<SnailNumber>(*other.parentSN);
        regularData = make_optional<int>(other.regularData.value());
    }
};

istream& operator >> (istream& is, SnailNumber &sn) {
    char nextChar;
    is >> nextChar; // which should be [
    sn.leftSN = make_shared<SnailNumber>(&sn);
    if (is.peek() == '[') {
        is >> *(sn.leftSN); // deepen @ left
    } else {
        int leftValue;  
        is >> leftValue;
        sn.leftSN->regularData = leftValue;
    }
    is >> nextChar; // which should be ,
    sn.rightSN = make_shared<SnailNumber>(&sn);
    if (is.peek() == '[') {
        is >> *sn.rightSN; // deepen @ right
    } else {
        int rightValue;
        is >> rightValue;
        sn.rightSN->regularData = rightValue;
    }
    is >> nextChar; // which should be ]
    return is;
}

ostream& operator << (ostream& os, SnailNumber const& sn) {
    if (sn.leftSN == nullptr || sn.rightSN == nullptr) {
        os << sn.regularData.value_or(-1);
        return os;
    }
    os << "[" << *(sn.leftSN) << ", " << *(sn.rightSN) << "]";
    return os;
}

SnailNumber operator + (SnailNumber& lhs, SnailNumber& rhs) {
    if (lhs.parentSN != nullptr || rhs.parentSN != nullptr) {
        throw (invalid_argument("LHS/RHS of + operator need to have no parent SNs"));
    }
    SnailNumber new_parent;
    lhs.parentSN = make_unique<SnailNumber>(new_parent);
    rhs.parentSN = make_unique<SnailNumber>(new_parent);
    new_parent.leftSN = make_unique<SnailNumber>(lhs);
    new_parent.rightSN = make_unique<SnailNumber>(rhs);
    return new_parent;
}

// Helper function: find most 
optional<SnailNumber&> findMostAdjacentOnLorR (SnailNumber& curr_sn, char L_OR_R) {
    if (L_OR_R == 'L') { // to explode left subtree, find adjacent left node
        SnailNumber* temp_parent = &curr_sn;
        while (temp_parent->parentSN->leftSN.get() == temp_parent) { 
            temp_parent = temp_parent->parentSN.get(); // backtrack up the tree until temp_parent is on the right to ITS parent
            if (temp_parent->parentSN == nullptr) { // when backtracked to top node
                return nullopt; // curr_sn is already leftmost leaf in the tree, return nullptr
            }
        }
        temp_parent = temp_parent->parentSN.get();
        SnailNumber* findFrom = temp_parent->leftSN.get();
        while (!findFrom->regularData.has_value()) { // then, get the rightmost RN on temp_parent.parentSN's left branch
            findFrom = findFrom->rightSN.get();
        }
        return (optional<SnailNumber&>(*findFrom));
    } else if (L_OR_R == 'R') { // to explode right subtree, find adjacent right node
        SnailNumber* temp_parent = &curr_sn;
        while (temp_parent->parentSN->rightSN.get() == temp_parent) { 
            temp_parent = temp_parent->parentSN.get(); // backtrack up the tree until temp_parent is on the left to ITS parent
            if (temp_parent->parentSN == nullptr) { // when backtracked to top node
                return nullopt; // curr_sn is already rightmost leaf in the tree, return nullptr
            }
        }
        temp_parent = temp_parent->parentSN.get();
        SnailNumber* findFrom = temp_parent->rightSN.get();
        while (!findFrom->regularData.has_value()) { // then, get the rightmost RN on temp_parent.parentSN's left branch
            findFrom = findFrom->leftSN.get();
        }
        return (optional<SnailNumber&>(*findFrom));
    } else {
        throw (invalid_argument("L_OR_R takes one of 'L' or 'R'"));
    }
}

// explode on leftmost base subtree (i.e., a non-regular node w/ left, right as regular number)
// when curr_sn is given at top-depth, depth = 1 (here depth is relative to entire tree)
// Looks slow... Exponential to tree depth
bool explode (SnailNumber& curr_sn, int depth = 1) {
    bool flag = false;
    if (!curr_sn.leftSN->regularData.has_value()) { // check left has subtree
        flag = (explode(*curr_sn.leftSN, depth + 1)); // expand on left subtree
    }
    if (!flag && !curr_sn.rightSN->regularData.has_value()) { // left has no subtree, check right has subtree
        flag = (explode(*curr_sn.rightSN, depth + 1)); // expand on right subtree
    }
    if (depth > 4) { // reg number on left and right of curr_sn, correct depth
        SnailNumber* parent_sn = curr_sn.parentSN.get();
        if (parent_sn->leftSN.get() == &curr_sn || 
            parent_sn->rightSN.get() == &curr_sn) { // curr_sn as left or right member of its parent
            // change adjacent left, then right regular SN (if exists)
            optional<SnailNumber> adj_leftRN = findMostAdjacentOnLorR(curr_sn, 'L'); // nullptr if curr_sn is already leftmost in the tree
            if (adj_leftRN.has_value()) { // if not leftmost, change adjacent left 
                adj_leftRN.value().regularData = adj_leftRN.value().regularData.value() + curr_sn.leftSN->regularData.value();
            } 
            optional<SnailNumber> adj_rightRN = findMostAdjacentOnLorR(curr_sn, 'R'); // likewise, nullptr if curr_sn is already rightmost in the tree
            if (adj_rightRN.has_value()) { // if not rightmost, change adjacent right
                adj_rightRN.value().regularData = adj_rightRN.value().regularData.value() + curr_sn.rightSN->regularData.value();
            }
            // change data of curr_sn itself
            curr_sn.regularData = 0;
            curr_sn.leftSN.reset();
            curr_sn.rightSN.reset();
        } else {
            throw runtime_error("curr_sn not on left nor right of its parent.");
        }
        flag = true;
        return flag;
    } else {
        return flag;
    }
}


/*
SnailNumber reduce (SnailNumber& top_sn) {

}*/

void test() {
    // struct io
    stringstream test_1 ("[1,1]");
    SnailNumber test_sn;
    test_1 >> test_sn;
    cout << test_sn << endl;

    SnailNumber test_sn_2;
    stringstream test_2 ("[[[[[9,8],1],2],3],4]");
    test_2 >> test_sn_2;
    cout << test_sn_2 << endl;

    // explode
    SnailNumber test_sn_3;
    stringstream test_3 ("[[[[[9,8],1],2],3],4]");
    test_3 >> test_sn_3;
    bool flag = explode(test_sn_3, 1);
    cout << test_sn_3 << endl;
    assert(flag == true);
}

int main() {
    test();
    return 0;
}





