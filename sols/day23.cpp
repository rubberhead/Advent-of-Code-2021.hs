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
    State(State& other) { // copy: maybe delete?
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

    State(vector<char> corridor, vector<Room> rooms, int64_t energy_used) { // New state
        this->corridor = corridor;
        this->rooms = rooms;
        this->energy_used = energy_used;
    }

    // State generation
    // Ideally state generation should be cached to reduce overhead... But my skill level isn't there yet
    vector<State> generateNextStates() {
        vector<State> nextstates;

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
            size_t to_corridor_cost = room_wo_top.size() - room_wo_top.inhabitants.size(); // Steps until at corridor[room_loc]
            // useful for both case 1a and 1b so here it is
            
            // 1a. into another (correct) room
            for (auto const& other : rooms) {
                if (&room == &other) { continue; } // 'other' point to same room as 'room'
                if (other.loc() < left_limit || other.loc() >= right_limit) { continue; } // 'other' unreachable from top in current state
                auto maybe_result = other.insert(top);
                if (maybe_result.has_value()) {
                    // 1. Compute energy delta
                    Room other_w_top = maybe_result.value();
                    size_t corridor_cost = other_w_top.loc() - room_wo_top.loc();
                    size_t from_corridor_cost = other.size() - other.inhabitants.size();
                    int64_t energy_delta = (to_corridor_cost + corridor_cost + from_corridor_cost) * ENERGY_RATE.at(top);
                    
                    // 2. Create new State instance accordingly
                    vector<Room> new_room_states = {room_wo_top, other_w_top};
                    for (auto const& unaffected : rooms) {
                        if (&unaffected == &room || &unaffected == &other) { continue; }
                        new_room_states.push_back(unaffected);
                    }
                    int64_t new_energy = energy_delta + this->energy_used;
                    State new_state (this->corridor, new_room_states, new_energy);
                    nextstates.push_back(new_state);
                }
            }
            // 1b. into corridor
            for (size_t i = left_limit; i < right_limit; i++) {
                if (any_of(rooms.begin(), rooms.end(), [&](Room& r){ return r.loc() == i; })) { continue; } 
                // top pod should not stop at any corridor positions leading to rooms

                // otherwise, 
                // 1. create new corridor situation
                vector<char> new_corridor (this->corridor);
                new_corridor[i] = top;

                // 2. Compute energy delta
                size_t corridor_cost = i - room_wo_top.loc();
                int64_t energy_delta = (to_corridor_cost + corridor_cost) * ENERGY_RATE.at(top);

                // 3. Create new State instance accordingly
                vector<Room> new_room_states = {room_wo_top};
                for (auto const& unaffected : rooms) {
                    if (&unaffected == &room) { continue; }
                    new_room_states.push_back(unaffected);
                }
                int64_t new_energy = energy_delta + this->energy_used;
                State new_state (new_corridor, new_room_states, new_energy);
                nextstates.push_back(new_state);
            }
        }

        // 2. Move pod from corridor to (correct) room that only contains correct pod residents
        for (size_t i = 0; i < corridor.size(); i++) {
            if (corridor[i] == '.') { continue; }

            // otherwise, put corridor[i] into correct room
            char pod = corridor[i];

            // Check movable range in the corridor
            size_t left_limit = i;
            while (left_limit >= 0 && corridor[left_limit] == '.') { left_limit--; }
            left_limit += 1; 
            size_t right_limit = i;
            while (right_limit < corridor.size() && corridor[right_limit] == '.') { right_limit++; }

            // Check room in range that also happens to be the correct room
            for (auto& room : rooms) {
                if (room.loc() >= left_limit && room.loc() < right_limit) {
                    auto maybe_room = room.insert(pod);
                    if (maybe_room.has_value()) {
                        // 1. Compute energy delta
                        Room room_w_i = maybe_room.value();
                        size_t corridor_cost = abs(static_cast<int64_t>(i) - static_cast<int64_t>(room_w_i.loc()));
                        size_t from_corridor_cost = room.size() - room.inhabitants.size();
                        int64_t energy_delta = (corridor_cost + from_corridor_cost) * ENERGY_RATE.at(pod);

                        // 2. Create new state
                        vector<char> new_corridor (corridor);
                        new_corridor[i] = '.';
                        vector<Room> new_room_states = {room_w_i};
                        for (auto& unaffected : rooms) {
                            if (&unaffected == &room) { continue; }
                            new_room_states.push_back(unaffected);
                        }
                        int64_t new_energy = energy_delta + this->energy_used;
                        State new_state (new_corridor, new_room_states, new_energy);
                        nextstates.push_back(new_state);
                    }
                }
            }
        }

        return nextstates;
    }

    // Goal check

};