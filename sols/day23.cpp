#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <memory>
#include <cassert>
using namespace std;

map<char, int64_t> const ENERGY_RATE = {
    {'A', 1}, 
    {'B', 10}, 
    {'C', 100}, 
    {'D', 1000}
};

struct Room {
private:
    char target;
    size_t size;
    size_t loc; // correspond to some idx in environment, non-mutable

public:
    stack<char> inhabitants;

    Room() = delete;
    Room(Room&&) = default;
    Room(Room& other) { // copy
        target = other.target;
        size = other.size;
        inhabitants = other.inhabitants;
        loc = other.loc;
    }
    Room(char const& target, size_t const& size, stack<char> const& inhabitants, size_t const& loc) { // use this for initialization
        this->target = target;
        this->size = size;
        this->inhabitants = inhabitants;
        this->loc = loc;
    }

    char target() const {
        return target;
    }

    size_t size() const {
        return size;
    }

    size_t loc() const {
        return loc;
    }

    bool insert(char& pod) {
        if (inhabitants.size() == size) {
            return false;
        }
        inhabitants.push(pod);
        return true;
    }

    

};

struct Environment {
    vector<char> corridor = {'.','.','.','.','.','.','.','.','.','.','.'};
    vector<Room> rooms;

    Environment() = delete;
    Environment(Environment&&) = default;
    Environment(Environment& other) { // copy
        this->corridor = other.corridor;
        this->rooms = other.rooms;
    }
    Environment(Room& rA, Room& rB, Room& rC, Room& rD) { // Initialization
        rooms.push_back(rA);
        rooms.push_back(rB);
        rooms.push_back(rC);
        rooms.push_back(rD);
    }

    vector<Environment> generateNextStates() {
        // 1. Move pod out of room
        for (auto& room : rooms) {
           // 1a. into another (correct) room
           // 1b. into corridor
        }

        // 2. Move pod from corridor to (correct) room
        
    }

};