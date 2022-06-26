#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <functional>
#include <cassert>
using namespace std;

struct BinaryBuffer {
    stringstream ins; // read-by-demand
    queue<bool> buffer;
    long read_count = 0;
    long size;

    BinaryBuffer(string const& _input) {
        ins << hex << _input;
        size = _input.size() * 4;
    }

    uint64_t read(size_t bit_count) {
        if (read_count > size) {
            throw (out_of_range("Read amount exceeds end of input"));
        }
        if (buffer.size() < bit_count) { // read new numbers from ins, code hell
            while (buffer.size() < bit_count) {
                char next = ins.get();
                if (next == EOF) {
                    throw invalid_argument("Input stream depleted, cannot read further");
                }
                vector<bool> translate;
                switch (next) { // dumb implementation
                    case '0':
                        translate = {0,0,0,0};
                        break;
                    case '1':
                        translate = {0,0,0,1};
                        break;
                    case '2':
                        translate = {0,0,1,0};
                        break;
                    case '3':
                        translate = {0,0,1,1};
                        break;
                    case '4':
                        translate = {0,1,0,0};
                        break;
                    case '5':
                        translate = {0,1,0,1};
                        break;
                    case '6':
                        translate = {0,1,1,0};
                        break;
                    case '7':
                        translate = {0,1,1,1};
                        break;
                    case '8':
                        translate = {1,0,0,0};
                        break;
                    case '9':
                        translate = {1,0,0,1};
                        break;
                    case 'A':
                        translate = {1,0,1,0};
                        break;
                    case 'B':
                        translate = {1,0,1,1};
                        break;
                    case 'C':
                        translate = {1,1,0,0};
                        break;
                    case 'D':
                        translate = {1,1,0,1};
                        break;
                    case 'E':
                        translate = {1,1,1,0};
                        break;
                    case 'F':
                        translate = {1,1,1,1};
                        break;
                    default:
                        throw (invalid_argument("Unexpected character: should be one of 0 to F in hex."));
                }
                for_each(translate.begin(), translate.end(), [&](auto bit){ buffer.push(bit); });
            }
        }
        uint64_t result = 0;
        while (bit_count > 0) {
            bool bit = buffer.front();
            result = result << 1 | bit;
            buffer.pop();
            read_count++;
            bit_count--;
        } 
        return result;
    }
};

struct Packet {
    uint64_t version;
    uint64_t id;
    bool ltype;
    uint64_t l_label;
    uint64_t c_label;
    //uint64_t data;

    Packet(uint64_t _version, uint64_t _id, uint64_t _ltype = -1, uint64_t _label = -1) {
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
    uint64_t version = currBuffer.read(3);
    uint64_t id = currBuffer.read(3);
    if (id == 4) { // literal
        return (make_unique<Packet>(Packet(version, id))); // went through 6 bits
    }
    uint64_t ltype = currBuffer.read(1);
    if (ltype) { // ltype == 1, read 11 as number of subpackets
        uint64_t c_label = currBuffer.read(11); 
        return (make_unique<Packet>(Packet(version, id, ltype, c_label))); // went through 18 bits
    }
    uint64_t l_label = currBuffer.read(15); // ltype == 0, read 15 as length of subpacks in total
    return (make_unique<Packet>(Packet(version, id, ltype, l_label))); // went through 22 bits
}

// getSol1
uint64_t readVersion(BinaryBuffer& currBuffer, Packet& currPacket) {
    if (currPacket.isLiteralPacket()) { 
        uint64_t result = currPacket.version;
        bool isLast = false;
        while (!isLast) {
            isLast = !(currBuffer.read(1)); // this hex-piece
            uint64_t data_piece = currBuffer.read(4);
        }
        return result;
    } else { // operator packet
        if (currPacket.ltype) { // 11-bit c_label, ready to read data
            uint64_t count_lim = currPacket.c_label;
            uint64_t result = currPacket.version;
            while (count_lim > 0) {
                // No ops defined yet so just read through the buffer w/o modifying result
                Packet subPacket = *(interpretType(currBuffer));
                result += readVersion(currBuffer, subPacket);
                count_lim--;
            }
            return result;
        } else { // 15-bit l_label, ready to read data
            uint64_t len_lim = currPacket.l_label;
            long curr_read_count = currBuffer.read_count;
            uint64_t result = currPacket.version;
            while (currBuffer.read_count < len_lim + curr_read_count) {
                Packet subPacket = *(interpretType(currBuffer)); // read_count may exceed size
                result += readVersion(currBuffer, subPacket);
            }
            return result;
        }
    }
}

uint64_t getVersionSum(string const& input) {
    BinaryBuffer inBuffer(input);
    Packet wholePacket = *interpretType(inBuffer);
    uint64_t result = readVersion(inBuffer, wholePacket); // moves buffer around both operator and literal types
    return result;
}

uint64_t operate(Packet& currPacket, vector<uint64_t>& result_buffer) {
    if (currPacket.isLiteralPacket()) {
        throw (invalid_argument("Literal packet as argument"));
    }
    uint64_t result = 0;
    switch (currPacket.id) {
        case 0: // sum
            result = accumulate(result_buffer.begin(), result_buffer.end(), result); // result == 0 of uint_64t size
            return result; // AAaaAAAAAAAAAAaaAAaAAaaAAAaAAAAAAaaaAAAAAAAaaA I FUCKING HATE COMPILER-MANAGED TYPE INTERPRETING
        case 1: // prod
            result = 1;
            result = accumulate(result_buffer.begin(), result_buffer.end(), result, multiplies<>()); // result == 1 of uint_64t size
            return result;
        case 2: // min
            return (*(min_element(result_buffer.begin(), result_buffer.end())));
        case 3: // max
            return (*(max_element(result_buffer.begin(), result_buffer.end())));
        case 5: // >; 5, 6, 7 packets have exactly two sub-packets
            return (static_cast<uint64_t>(result_buffer[0]) > static_cast<uint64_t>(result_buffer[1]));
        case 6: // <
            return (static_cast<uint64_t>(result_buffer[0]) < static_cast<uint64_t>(result_buffer[1]));
        case 7: // ==
            return (static_cast<uint64_t>(result_buffer[0]) == static_cast<uint64_t>(result_buffer[1]));
    }
    throw (invalid_argument("Packet ID undefined"));
}

uint64_t accumulateValue(BinaryBuffer& currBuffer, Packet& currPacket) {
    if (currPacket.isLiteralPacket()) { 
        uint64_t result = 0;
        bool isLast = false;
        while (!isLast) {
            isLast = !(currBuffer.read(1)); // this hex-piece
            uint64_t data_piece = currBuffer.read(4);
            result = result << 4 | data_piece; 
        }
        return result;
    } else { // operator packet
        if (currPacket.ltype) { // 11-bit c_label, ready to read data
            uint64_t count_lim = currPacket.c_label;
            vector<uint64_t> result_buffer;
            while (count_lim > 0) {
                Packet subPacket = *(interpretType(currBuffer));
                result_buffer.push_back(accumulateValue(currBuffer, subPacket));
                count_lim--;
            }
            return (operate(currPacket, result_buffer));
        } else { // 15-bit l_label, ready to read data
            uint64_t len_lim = currPacket.l_label;
            long curr_read_count = currBuffer.read_count;
            vector<uint64_t> result_buffer;
            while (currBuffer.read_count < len_lim + curr_read_count) {
                Packet subPacket = *(interpretType(currBuffer)); // read_count may exceed size
                result_buffer.push_back(accumulateValue(currBuffer, subPacket));
            }
            return (operate(currPacket, result_buffer));
        }
    }
}

uint64_t evaluateString(string const& input) {
    BinaryBuffer inBuffer(input);
    Packet wholePacket = *interpretType(inBuffer);
    uint64_t result = accumulateValue(inBuffer, wholePacket);
    return result;
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
    /*
    string test_5 = "C200B40A82";
    assert(evaluateString(test_5) == 3);
    string test_6 = "CE00C43D881120";
    assert(evaluateString(test_6) == 9);
    string test_7 = "9C0141080250320F1802104A08";
    assert(evaluateString(test_7) == 1);
    string test_8 = "9C005AC2F8F0";
    assert(evaluateString(test_8) == 0);
    string test_9 = "D2FE28"; // literal
    assert(evaluateString(test_9) == 2021);
    string test_10 = "880086C3E88112";
    assert(evaluateString(test_10) == 7);
    string test_11 = "04005AC33890";
    assert(evaluateString(test_11) == 54); 
    string test_12 = "3232D42BF9400";
    assert(evaluateString(test_12) == 5000000000); */

    // custom test, generated using https://davidyue.live/aoc/app/packetcode.html
    string test_a = "2200CC877338F9664EB93950025C00950025C009500660CC21B004614C30";
    assert(evaluateString(test_a) == 14247810);
    string test_b = "22008C9CB31ED370A6E7A9A0C876B7D56B1EF2DD08";
    assert(evaluateString(test_b) == 14468812903088393);
}

int main() {
    test();

    string input;
    ifstream inputf ("../resource/q16/input");
    inputf >> input;

    // 1.
    cout << "Part 1: " << getVersionSum(input) << endl;

    // 2.
    cout << "Part 2: " << evaluateString(input) << endl;

    return 0;
}

