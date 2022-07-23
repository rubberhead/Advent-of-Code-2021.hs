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
        _loc = other._loc;
        for (char const c : other.inhabitants) {
            inhabitants.push_back(c);
        }
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
        if (any_of(inhabitants.begin(), inhabitants.end(), [&](char const& resident) { return resident != pod; })) {
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
    State(State const& other) {
        this->corridor = other.corridor;
        this->energy_used = other.energy_used;
        for (Room const& room : other.rooms) {
            this->rooms.push_back(Room(forward<Room const&>(room)));
        }
    }
    State(Room rA, Room rB, Room rC, Room rD) { // Initialization by rvalue
        rooms.push_back(rA);
        rooms.push_back(rB);
        rooms.push_back(rC);
        rooms.push_back(rD);
        energy_used = 0;
    }
    State(Room& rA, Room& rB, Room& rC, Room& rD) { // Initialization by reference
        rooms.push_back(rA);
        rooms.push_back(rB);
        rooms.push_back(rC);
        rooms.push_back(rD);
        energy_used = 0;
    }

    State(vector<char> corridor, vector<Room> rooms, int64_t energy_used) { // New state
        this->corridor = corridor;
        this->energy_used = energy_used;
        for (Room& room : rooms) {
            this->rooms.push_back(move(room));
        }
    }

    // State generation
    // Ideally state generation should be cached to reduce overhead... But my skill level isn't there yet
    vector<State> generateNextStates() {
        vector<State> nextstates;

        // 1. Move top pod out of room
        for (auto const& room : rooms) {
            if (room.inhabitants.empty()) { continue; } // seem to have issues w/ copying empty deques, no idea why

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
            int64_t to_corridor_cost = room_wo_top.size() - room_wo_top.inhabitants.size(); // Steps until at corridor[room_loc]
            // useful for both case 1a and 1b so here it is
            
            // 1a. into another (correct) room
            for (auto const& other : rooms) {
                if (&room == &other) { continue; } // 'other' point to same room as 'room'
                if (other.loc() < left_limit || other.loc() >= right_limit) { continue; } // 'other' unreachable from top in current state
                auto maybe_result = other.insert(top);
                if (maybe_result.has_value()) {
                    // 1. Compute energy delta
                    Room other_w_top = maybe_result.value();
                    int64_t corridor_cost = other_w_top.loc() - room_wo_top.loc();
                    int64_t from_corridor_cost = other.size() - other.inhabitants.size();
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
                int64_t corridor_cost = abs(static_cast<int64_t>(i) - static_cast<int64_t>(room_wo_top.loc()));
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
            // char pod = corridor[i];

            // Check movable range in the corridor
            size_t left_limit = i;
            while (left_limit >= 0 && corridor[left_limit] == '.') { left_limit--; }
            left_limit += 1; 
            size_t right_limit = i;
            while (right_limit < corridor.size() && corridor[right_limit] == '.') { right_limit++; }

            // Check room in range that also happens to be the correct room
            for (auto& room : rooms) {
                if (room.loc() >= left_limit && room.loc() < right_limit) {
                    auto maybe_room = room.insert(corridor[i]);
                    if (maybe_room.has_value()) {
                        // 1. Compute energy delta
                        Room room_w_i = maybe_room.value();
                        int64_t corridor_cost = abs(static_cast<int64_t>(i) - static_cast<int64_t>(room_w_i.loc()));
                        int64_t from_corridor_cost = room.size() - room.inhabitants.size();
                        int64_t energy_delta = (corridor_cost + from_corridor_cost) * ENERGY_RATE.at(corridor[i]);

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
    // if this state is goal, return energy state, otherwise return nullopt
    optional<int64_t> isGoal() {
        if (all_of(rooms.begin(), rooms.end(), [](Room const& room) {
                return ( all_of(room.inhabitants.begin(), room.inhabitants.end(), [&](char const& resident) { return resident == room.target(); }) );
            }) &&
            all_of(corridor.begin(), corridor.end(), [](char const c) { return c == '.'; })
        ) {
            return (make_optional<int64_t>(this->energy_used));
        }
        return nullopt;
    }
};

// Complete state-space search
int64_t simpleSearch(State& source) {
    deque<State> states {source};
    int64_t least_energy = INT64_MAX;
    while(!states.empty()) {
        State s = states.front();
        states.pop_front();
        auto maybe_energy = s.isGoal();
        if (maybe_energy.has_value()) {
            if (maybe_energy.value() < least_energy) {
                least_energy = maybe_energy.value();
            }
        } else {
            vector<State> buffer = s.generateNextStates();
            for_each(buffer.begin(), buffer.end(), [&](State& s) { states.push_back(s); });
        }
    }
    return least_energy;
}

void test() {
    // State configuration
    State test_state (Room('A', 2, {'B', 'A'}, 2), Room('B', 2, {'C', 'D'}, 4), Room('C', 2, {'B', 'C'}, 6), Room('D', 2, {'D', 'A'}, 8));
    assert(all_of(test_state.rooms.begin(), test_state.rooms.end(), [](Room const& room) { return room.inhabitants.size() == room.size(); }));
    assert(all_of(test_state.corridor.begin(), test_state.corridor.end(), [](char const& c){ return c == '.'; }));
    assert(test_state.energy_used == 0);
    
    // next state generation
    vector<State> test_nexts = test_state.generateNextStates();
    State test_state_1 (
        Room('A', 2, {'A'}, 2), 
        Room('B', 2, {'B', 'B'}, 4), 
        Room('C', 2, {'C', 'C'}, 6),
        Room('D', 2, {'D', 'D'}, 8)
    );
    test_state_1.corridor[0] = 'A';
    test_nexts = test_state_1.generateNextStates();
    // cout << "Generated" << endl;

    // goal check
    State test_goal (Room('A', 2, {'A', 'A'}, 2), Room('B', 2, {'B', 'B'}, 4), Room('C', 2, {'C', 'C'}, 6), Room('D', 2, {'D', 'D'}, 8));
    assert(test_goal.isGoal().value() == 0);

    // Simple state space search
    int64_t test_result = simpleSearch(test_state);
    assert(test_result == 12521);
}

int main() {
    test();
    return 0;
}