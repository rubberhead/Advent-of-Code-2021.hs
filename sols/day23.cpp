#include <iostream>
#include <stack>
#include <deque>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <algorithm>
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
    char _target;
    size_t _size;
    size_t _loc; // correspond to some idx in environment, non-mutable

public:
    deque<char> inhabitants;

    Room() = delete;
    Room(Room&&) = default;
    Room(Room const& other) { // copy
        _target = other._target;
        _size = other._size;
        inhabitants = other.inhabitants;
        _loc = other._loc;
    }
    Room(char const& target, size_t const& size, deque<char> const& inhabitants, size_t const& loc) { // use this for initialization
        this->_target = target;
        this->_size = size;
        this->inhabitants = inhabitants;
        this->_loc = loc;
    }

    char target() const {
        return _target;
    }

    size_t size() const {
        return _size;
    }

    // getter
    size_t loc() const {
        return _loc;
    }

    optional<Room> insert(char& pod) const {
        if (pod != _target) { return nullopt; } // pod not intended for this room
        if (inhabitants.size() == _size) { return nullopt; } // room full
        if (any_of(inhabitants.begin(), inhabitants.end(), [&](char& resident) { return resident != pod; })) {
            return nullopt; // pod intended for this room, but room has resident of incorrect type
        }
        Room result (*this); // otherwise, create resultant room
        result.inhabitants.push_front(pod);
        return (make_optional<Room>(result));
    }
};

struct State {
    vector<char> corridor = {'.','.','.','.','.','.','.','.','.','.','.'};
    vector<Room> rooms;
    int64_t energy_used;

    State() = delete;
    State(State&&) = default;
    State(State& other) { // copy
        this->corridor = other.corridor;
        this->rooms = other.rooms;
        this->energy_used = other.energy_used;
    }
    State(Room& rA, Room& rB, Room& rC, Room& rD) { // Initialization
        rooms.push_back(rA);
        rooms.push_back(rB);
        rooms.push_back(rC);
        rooms.push_back(rD);
        energy_used = 0;
    }

    vector<State> generateNextStates() {
        vector<State> nextStates;

        // 1. Move top pod out of room
        for (auto const& room : rooms) {
            // Check movable range in the corridor
            size_t left_limit = room.loc();
            while (left_limit >= 0 && corridor[left_limit] == '.') { left_limit--; }
            left_limit += 1; 
            size_t right_limit = room.loc();
            while (right_limit < corridor.size() && corridor[right_limit] == '.') { right_limit++; }

            // Move top pod out of room
            Room room_wo_top (room);
            char& top = room_wo_top.inhabitants.front();
            room_wo_top.inhabitants.pop_front();
            
            // 1a. into another (correct) room
            for (auto const& other : rooms) {
                if (&room == &other) { continue; } // 'other' point to same room as 'room'
                if (other.loc() < left_limit || other.loc() >= right_limit) { continue; } // 'other' unreachable from top in current state
                auto maybe_result = other.insert(top);
                if (maybe_result.has_value()) {
                    // 1. Compute energy delta
                    size_t to_corridor_cost = room_wo_top.size() - room_wo_top.inhabitants.size(); // Now at corridor[room_loc]
                    Room other_w_top = maybe_result.value();
                    size_t corridor_cost = other_w_top.loc() - room_wo_top.loc();
                    size_t from_corridor_cost = other.size() - other.inhabitants.size();
                    int64_t energy_delta = (to_corridor_cost + corridor_cost + from_corridor_cost) * ENERGY_RATE.at(top);
                    
                    // 2. Create new State instance accordingly
                }
            }
            // 1b. into corridor
        }

        // 2. Move pod from corridor to (correct) room
        // that only contains correct pod residents
        
    }

};