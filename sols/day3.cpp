#include <iostream>
#include <fstream>
#include <cmath>
#include <bitset>
using namespace std;

// Singular linked list
class Node {
    public:
        int data;
        Node *next; // ptr to next node in line

        // constructor
        Node(int _data) {
            data = _data;
            next = NULL;
        }

        // delete next element, return pointer to deleted element
        Node* deleteNext() {
            Node* ptr = next;
            if (next != NULL) {
                cout << "Deleted " << ptr << " with value " << ptr->data << endl;
                next = next->next;
                cout << "New next is " << next << endl;
                return ptr;
            } else {
                cout << "Deleted nothing. Ptr already at end." << endl;
                return nullptr;
            }
        }
};

// Part 1
// gamma - epsilon rate
// gamma rate computed from taking the most common bit for each column in 
// in all lines.
// epsilon rate computed from taking the least common bit ...
// Since we use binary system epsilon could be computed with inversion.

// Return the decimal results of two numbers multiplied together.
long getSol1(string filepath) {
    ifstream inputf (filepath);
    
    string nextline;
    getline (inputf, nextline);
    long size = nextline.length();
    long count[size] = {}; // keeps track of number of 0s in each column
    // at the end, compare each element to ceil(n / 2) where n is number of lines.
    // iff smaller then the corresp. bit for gamma should be 1. Else 0.
    long lines_in_file = 1;

    // Find number of 0s
    while (inputf.peek() != EOF) {
        long i = 0;
        while (i < size) {
            if (nextline[i] == '0') { // Assume all lines have equal size
                count[i]++;
            }
            i++;
        }
        lines_in_file++;
        getline (inputf, nextline); // Failure: Last line not operated on
    }
    inputf.close();

    // Query: is 0 more popular? -> epsilon
    long epsilon = 0B0;
    for (long i = 0; i < size; i++) {
        if (count[i] < ceil(lines_in_file / 2.0)) {
            count[i] = 0;
        } else {
            count[i] = 1;
            // set epsilon bit at (size - 1 - i)
            epsilon |= 1UL << (size - 1 - i);
            // reads "epsilon ENTAILS unsigned long 1 left shifted by (size - 1 - i) bits"
        }
    }
    // Then take masked inverse for gamma (limited to last <size> bits) using XOR
    long mask  = exp2(size) - 1;
    long gamma = epsilon ^ mask;

    bitset<32> e_b (epsilon);
    cout << "Epsilon " << e_b << endl;
    bitset<32> g_b (gamma);
    cout << "Gamma   " << g_b << endl;

    // Return
    return (epsilon * gamma);
}

// Part 2
// Oxygen - CO2 rate
long getSol2(string filepath) {
    ifstream inputf (filepath);
    string nextline;

    // Iterate through file to get all numbers
    // as well as most common *first* bit
    Node number_pool_head (-1);
    Node* number_pool_curr = &number_pool_head; // iterated

    int zero_bit_count = 0;
    int one_bit_count = 0;

    cout << "Doing iteration" << endl;
    while (inputf.peek() != EOF) {
        getline (inputf, nextline);
        // Count bit occurrence
        if (nextline[0] == '0') {
            zero_bit_count++;
        } else {
            one_bit_count++;
        }

        // Fill number into pool
        try {
            number_pool_curr->next = new Node(stoi(nextline, 0, 2));
            number_pool_curr = number_pool_curr->next;
        } catch (invalid_argument& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }

    inputf.close();
    
    // Filtering
    cout << "Doing filtering" << endl;
    int number_bit_size = nextline.length(); // nextline still stores last line
    int filter_on = 0; // left to right
    Node* oxy_pool_head = &number_pool_head;
    Node co2_pool_head (-1);

    // Initialize oxy_pool and co2_pool using elems from number_pool
    // i.e., filter on first bit
    Node* oxy_pool_curr = oxy_pool_head;
    Node* co2_pool_curr = &co2_pool_head;
    while (oxy_pool_curr->next != NULL) { // starting from placeholder
        //cout << "Checking " << oxy_pool_curr->next << " with value " << oxy_pool_curr->next->data << endl;
        if (zero_bit_count >= one_bit_count) {
            //cout << "Because 0 is majority at bit " << filter_on << endl;
            if (oxy_pool_curr->next->data >> (number_bit_size - 1 - filter_on) & 1) { // i.e., 1 at that bit
                Node* co2_pool_next = oxy_pool_curr->deleteNext();
                co2_pool_curr->next = co2_pool_next;
                co2_pool_curr = co2_pool_next;
            } else {
                oxy_pool_curr = oxy_pool_curr->next;
                continue;
            }
        } else { // 1-bit-count > 0-bit-count
            //cout << "Because 1 is majority at bit " << filter_on << endl;
            if (oxy_pool_curr->next->data >> (number_bit_size - 1 - filter_on) & 1) { // i.e., 1 at that bit
                oxy_pool_curr = oxy_pool_curr->next;
                continue;
            } else {
                Node* co2_pool_next = oxy_pool_curr->deleteNext();
                co2_pool_curr->next = co2_pool_next;
                co2_pool_curr = co2_pool_next;
            }
        }
    }
    //cout << co2_pool_curr->next << endl;

    // Iterate on both LLs w/ incrementing filter_on
    // Before filtering, update 0, 1 bit counts correspondingly
    oxy_pool_curr = oxy_pool_head;
    co2_pool_curr = &co2_pool_head;
    int oxy_0_bit_count = 0;
    int oxy_1_bit_count = 0;
    int co2_0_bit_count = 0;
    int co2_1_bit_count = 0;
    filter_on++;

    while (!(oxy_pool_head->next->next == NULL && 
           co2_pool_head.next->next == NULL) && 
           filter_on < number_bit_size) {
        cout << "========================================================" << endl;
        cout << "Filtering on bit " << filter_on << " from L to R" << endl;

        // Update count for oxy
        cout << "Updating OXY" << endl;
        // Print - iterate
        oxy_pool_curr = oxy_pool_head;
        while (oxy_pool_curr != NULL) {
            cout << oxy_pool_curr << " contains " << oxy_pool_curr->data << endl;
            oxy_pool_curr = oxy_pool_curr->next;
        }
        oxy_pool_curr = oxy_pool_head;
        while (oxy_pool_curr->next != NULL) {
            int candidate = oxy_pool_curr->next->data;
            if (candidate >> (number_bit_size - 1 - filter_on) & 1) {
                oxy_1_bit_count++;
            } else {
                oxy_0_bit_count++;
            }
            oxy_pool_curr = oxy_pool_curr->next;
        }
        // Filter oxy accordingly
        oxy_pool_curr = oxy_pool_head;
        while (oxy_pool_curr->next != nullptr) {
            if (oxy_pool_curr == oxy_pool_head && oxy_pool_curr->next->next == nullptr) {
                break;
            }
            cout << "------------------------------------------------------------------" << endl;
            if (oxy_0_bit_count > oxy_1_bit_count) { // prefer 1
                cout << "Because 0 is majority at bit " << filter_on << " with " << oxy_0_bit_count << "counts" << endl;
                if (oxy_pool_curr->next->data >> (number_bit_size - filter_on - 1) & 1) { // i.e., 1 at that bit
                    oxy_pool_curr->deleteNext();
                } else {
                    cout << "Retain " << oxy_pool_curr->next << " with value " << oxy_pool_curr->next->data << endl;
                    oxy_pool_curr = oxy_pool_curr->next;
                }
            } else { // <=
                cout << "Because 1 is majority at bit " << filter_on << " with " << oxy_1_bit_count << "counts" << endl;
                if (oxy_pool_curr->next->data >> (number_bit_size - filter_on - 1) & 1) { // i.e., 1 at that bit
                    cout << "Retain " << oxy_pool_curr->next << " with value " << oxy_pool_curr->next->data << endl;
                    oxy_pool_curr = oxy_pool_curr->next;
                } else {
                    oxy_pool_curr->deleteNext();
                }
            }
        }

        // Update count for co2
        cout << "Updating CO2" << endl;
        // Print - iterate
        co2_pool_curr = &co2_pool_head;
        while (co2_pool_curr != NULL) {
            cout << co2_pool_curr << " contains " << co2_pool_curr->data << endl;
            co2_pool_curr = co2_pool_curr->next;
        }
        co2_pool_curr = &co2_pool_head;
        while (co2_pool_curr->next != NULL) {
            int candidate = co2_pool_curr->next->data;
            if (candidate >> (number_bit_size - 1 - filter_on) & 1) {
                co2_1_bit_count++;
            } else {
                co2_0_bit_count++;
            }
            co2_pool_curr = co2_pool_curr->next;
        }
        // Filter co2 accordingly
        co2_pool_curr = &co2_pool_head;
        while (co2_pool_curr->next != nullptr) {
            if (co2_pool_curr == &co2_pool_head && co2_pool_curr->next->next == nullptr) {
                break;
            }
            cout << "------------------------------------------------------------------" << endl;
            if (co2_0_bit_count > co2_1_bit_count) { // prefer 0
                cout << "Because 0 is majority at bit " << filter_on << " for CO2" << endl;
                if (co2_pool_curr->next->data >> (number_bit_size - filter_on - 1) & 1) { // i.e., 1 at that bit
                    cout << "Retain " << co2_pool_curr->next << " with value " << co2_pool_curr->next->data << endl;
                    cout << "New next is " << co2_pool_curr->next << endl;
                    co2_pool_curr = co2_pool_curr->next;
                } else {
                    co2_pool_curr->deleteNext();
                }
            } else { // <
                cout << "Because 1 is majority at bit " << filter_on << " for CO2" << endl;
                if (co2_pool_curr->next->data >> (number_bit_size - filter_on - 1) & 1) { // i.e., 1 at that bit
                    co2_pool_curr->deleteNext();
                } else {
                    cout << "Retain " << co2_pool_curr->next << " with value " << co2_pool_curr->next->data << endl;
                    cout << "New next is " << co2_pool_curr->next << endl;
                    co2_pool_curr = co2_pool_curr->next;
                }
            }
        }
        oxy_0_bit_count = 0;
        oxy_1_bit_count = 0;
        co2_0_bit_count = 0;
        co2_1_bit_count = 0;
        filter_on++;
    }

    cout << "Finally we get " << oxy_pool_head->next->data << " for OXY and " << co2_pool_head.next->data << " for CO2" << endl;
    return (oxy_pool_head->next->data * co2_pool_head.next->data);
}

int main() {
    cout << getSol1("../resource/q3/input") << endl;
    cout << getSol2("../resource/q3/input") << endl;
    return 0;
}