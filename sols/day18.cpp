#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <optional>
#include <memory>
#include <cassert>
using namespace std;

struct SnailNumber { // largely followed code structure from Simon Toth's blog
    unique_ptr<SnailNumber> leftSN;
    unique_ptr<SnailNumber> rightSN;
    unique_ptr<SnailNumber> parentSN;
    optional<int> regularData;

    SnailNumber() = default;
    SnailNumber(SnailNumber &&) = default;
    SnailNumber& operator = (SnailNumber &&) = default;

    SnailNumber(unique_ptr<SnailNumber>& _parent_ptr) {
        parentSN = make_unique<SnailNumber>(*_parent_ptr);
    }

    SnailNumber(int _regularData, unique_ptr<SnailNumber>& _parent_ptr) {
        regularData = _regularData;
        parentSN = make_unique<SnailNumber>(*_parent_ptr);
    }

    SnailNumber(SnailNumber const& other){
        leftSN = make_unique<SnailNumber>(*other.leftSN);
        rightSN = make_unique<SnailNumber>(*other.rightSN);
        parentSN = make_unique<SnailNumber>(*other.parentSN);
        regularData = make_optional<int>(other.regularData.value());
    }
};

istream& operator >> (istream& is, SnailNumber &sn) {
    char nextChar;
    is >> nextChar; // which should be [
    sn.leftSN = make_unique<SnailNumber>(make_unique<SnailNumber>(sn));
    if (is.peek() == '[') {
        is >> *sn.leftSN; // deepen @ left
    } else {
        int leftValue;
        is >> leftValue;
        sn.leftSN->regularData = leftValue;
    }
    is >> nextChar; // which should be ,
    sn.rightSN = make_unique<SnailNumber>(make_unique<SnailNumber>(sn));
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

SnailNumber reduce (SnailNumber& top_sn) {

}

// explode on leftmost base subtree (i.e., a non-regular node w/ left, right as regular number)
// when curr_sn is given at top-depth, depth = 1 (here depth is relative to entire tree)
bool explode (SnailNumber& curr_sn, int depth = 1) {
    bool flag = false;
    if (!curr_sn.leftSN->regularData.has_value()) { // left has subtree
        flag = (explode(*curr_sn.leftSN, depth + 1)); // expand on left subtree
    }
    if (!curr_sn.rightSN->regularData.has_value()) { // right has subtree
        flag = (explode(*curr_sn.rightSN, depth + 1)); // expand on right subtree
    }
    if (depth > 4) {
        SnailNumber parent_sn = *curr_sn.parentSN;
        
    } else {
        return flag;
    }
}

// 
unique_ptr<SnailNumber> findMostAdjacentOnLorR (SnailNumber& curr_sn, char L_OR_R) {
    if (L_OR_R == 'L') { // explode left subtree, find adjacent left node
        SnailNumber temp_parent = curr_sn.parentSN;
        while (temp_parent.parentSN->leftSN == make_unique<SnailNumber>(temp_parent)) {
            temp_parent = temp_parent.parentSN;
        }
        temp_parent = temp_parent.parentSN;
        SnailNumber findFrom = temp_parent.leftSN;
        while (!findFrom.regularData.has_value()) {
            findFrom = findFrom.rightSN;
        }
        return (make_unique<SnailNumber>(findFrom));
    } else if (L_OR_R == 'R') { // explode right subtree, find adjacent right node
        SnailNumber temp_parent = curr_sn.parentSN;
        while (temp_parent.parentSN->rightSN == make_unique<SnailNumber>(temp_parent)) {
            temp_parent = temp_parent.parentSN;
        }
        temp_parent = temp_parent.parentSN;
        SnailNumber findFrom = temp_parent.rightSN;
        while (!findFrom.regularData.has_value()) {
            findFrom = findFrom.leftSN;
        }
        return (make_unique<SnailNumber>(findFrom));
    } else {
        throw (invalid_argument("L_OR_R takes one of 'L' or 'R'"));
    }
}


void test() {
    stringstream test_1 ("[1,1]");
    SnailNumber test_sn;
    test_1 >> test_sn;
    cout << test_sn << endl;
    stringstream test_2 ("[[[[[9,8],1],2],3],4]");
    test_2 >> test_sn;
    cout << test_sn << endl;
}

int main() {
    test();
    return 0;
}





