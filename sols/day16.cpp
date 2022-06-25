#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <bitset>
#include <memory>
#include <cassert>
using namespace std;

struct BinaryBuffer {
    stringstream ins; // read-by-demand
    queue<bool> buffer;
    int read_count = 0;
    int size;

    BinaryBuffer(string const& _input) {
        ins << hex << _input;
        size = _input.size() * 4;
    }

    unsigned long read(size_t bit_count) {
        if (read_count > size) {
            throw (out_of_range("Buffer read to end of input"));
        }
        if (buffer.size() < bit_count) { // read new numbers from ins
            while (buffer.size() < bit_count) {
                char next = ins.get();
                if (next == EOF) {
                    throw invalid_argument("Input stream depleted, cannot read further");
                }

                unsigned int nextint = strtol(&next, nullptr, 16);
                if (nextint >= 8) {
                    // 1xxx
                    buffer.push(1);
                    nextint -= 8;
                } else {
                    buffer.push(0);
                }

                if (nextint >= 4) {
                    // x1xx
                    buffer.push(1);
                    nextint -= 4;
                } else {
                    buffer.push(0);
                }

                if (nextint >= 2) {
                    // xx1x
                    buffer.push(1);
                    nextint -= 2;
                } else {
                    buffer.push(0);
                }

                if (nextint >= 1) {
                    // xxx1
                    buffer.push(1);
                    nextint -= 1;
                } else {
                    buffer.push(0);
                }
                if (nextint != 0) {
                    throw invalid_argument("Unexpected number: faulty computation for hex->bin conversion");
                }
            }
        }
        unsigned long result = 0;
        while (bit_count > 0) {
            bool bit = buffer.front();
            result <<= 1;
            result += bit;
            buffer.pop();
            read_count++;
            bit_count--;
        } 
        return result;
    }
};

struct Packet {
    unsigned version;
    unsigned id;
    bool ltype;
    unsigned l_label;
    unsigned c_label;
    unsigned data;

    Packet(unsigned _version, unsigned _id, unsigned _ltype = -1, unsigned _label = -1) {
        version = _version;
        id = _id;
        if (id == 4) { // literal, c_label == 1
            l_label = 0;
            c_label = 1;
            return;
        }
        ltype = _ltype;
        if (ltype) { // ltype == 1, use c_label (11-bit)
            c_label = _label;
            l_label = 0;
            return;
        }
        l_label = _label; // ltype == 0, use l_label (15-bit)
        c_label = 0;
    }

    bool isLiteralPacket() {
        return (id == 4);
    }
};

// Read the given buffer and return a new Packet unique_ptr
// that contains information starting from next bit of the buffer.
// After call, buffer should be aligned at one of:
// a. next subpacket (if packet is of operator type)
// b. literal information (if packet is of literal type)
unique_ptr<Packet> interpretType(BinaryBuffer& currBuffer) {
    unsigned version = currBuffer.read(3);
    unsigned id = currBuffer.read(3);
    if (id == 4) { // literal
        return (make_unique<Packet>(Packet(version, id))); // went through 6 bits
    }
    unsigned ltype = currBuffer.read(1);
    if (ltype) { // ltype == 1, read 11 as number of subpackets
        unsigned c_label = currBuffer.read(11); 
        return (make_unique<Packet>(Packet(version, id, ltype, c_label))); // went through 18 bits
    }
    unsigned l_label = currBuffer.read(15); // ltype == 0, read 15 as length of subpacks in total
    return (make_unique<Packet>(Packet(version, id, ltype, l_label))); // went through 22 bits
}

// getSol1
unsigned int readVersion(BinaryBuffer& currBuffer, Packet& currPacket) {
    if (currPacket.isLiteralPacket()) { 
        unsigned result = currPacket.version;
        bool isLast = false;
        while (!isLast) {
            isLast = !(currBuffer.read(1)); // this hex-piece
            unsigned data_piece = currBuffer.read(4);
            // result <<= 4; // why the hell did I do this this should be for when reading actual data not here
            // result += data_piece;
        }
        return result;
    } else { // operator packet
        if (currPacket.ltype) { // 11-bit c_label, ready to read data
            unsigned count_lim = currPacket.c_label;
            unsigned result = currPacket.version;
            while (count_lim > 0) {
                // No ops defined yet so just read through the buffer w/o modifying result
                Packet subPacket = *(interpretType(currBuffer));
                result += readVersion(currBuffer, subPacket);
                count_lim--;
            }
            return result;
        } else { // 15-bit l_label, ready to read data
            unsigned len_lim = currPacket.l_label;
            int curr_read_count = currBuffer.read_count;
            unsigned result = currPacket.version;
            while (currBuffer.read_count < len_lim + curr_read_count) {
                Packet subPacket = *(interpretType(currBuffer)); // read_count may exceed size
                result += readVersion(currBuffer, subPacket);
            }
            return result;
        }
    }
}

unsigned int getVersionSum(string const& input) {
    BinaryBuffer inBuffer(input);
    Packet wholePacket = *interpretType(inBuffer);
    unsigned int result = readVersion(inBuffer, wholePacket); // moves buffer around both operator and literal types
    return result;
}

// getSol2
unsigned int evaluate(BinaryBuffer& currBuffer, Packet& currPacket) {
    if (currPacket.isLiteralPacket()) { 
        unsigned result = 0;
        bool isLast = false;
        while (!isLast) {
            isLast = !(currBuffer.read(1)); // this hex-piece
            unsigned data_piece = currBuffer.read(4);
            result <<= 4; // why the hell did I do this this should be for when reading actual data not here
            result += data_piece;
        }
        return result;
    } else { // operator packet
        if (currPacket.ltype) { // 11-bit c_label, ready to read data
            unsigned count_lim = currPacket.c_label;
            unsigned result = 0;
            while (count_lim > 0) {
                Packet subPacket = *(interpretType(currBuffer));
                // switch - case, best written outside cuz both cases need it identically
                count_lim--;
            }
            return result;
        } else { // 15-bit l_label, ready to read data
            unsigned len_lim = currPacket.l_label;
            int curr_read_count = currBuffer.read_count;
            unsigned result = currPacket.version;
            while (currBuffer.read_count < len_lim + curr_read_count) {
                Packet subPacket = *(interpretType(currBuffer)); // read_count may exceed size
                result += readVersion(currBuffer, subPacket);
            }
            return result;
        }
    }
}

void test() {
    string test_1 = "38006F45291200";

    // BinaryBuffer read
    BinaryBuffer test_1_buffer (test_1);
    assert(test_1_buffer.read(3) == 0b001);
    assert(test_1_buffer.read(3) == 0b110);
    test_1_buffer = BinaryBuffer(test_1);
    assert(test_1_buffer.read(56) == 0x38006F45291200); // BEWARE! Currently there is no end check for BinaryBuffer instances

    // Read version
    string test_2 = "8A004A801A8002F478";
    assert(getVersionSum(test_2) == 16);
    string test_3 = "620080001611562C8802118E34";
    assert(getVersionSum(test_3) == 12);
    string test_4 = "C0015000016115A2E0802F182340";
    assert(getVersionSum(test_4) == 23);

    // Evaluate

}

int main() {
    test();

    string input;
    ifstream inputf ("../resource/q16/input");
    inputf >> input;

    // 1.
    cout << "Part 1: " << getVersionSum(input) << endl;

    // 2.


    return 0;
}

