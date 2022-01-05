// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 23 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the least energy required to organize the amphipods?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>
#include <map>
#include <stack>
#include <queue>
// TODO FINALLY remove debugging statements

using namespace std;

enum AmphipodType { amber=0, bronze=1, copper=2, desert=3, AMPHIPOD_SIZE=4 };

class Amphipod {
  private:
    AmphipodType type;

    void copy(const Amphipod& other) {
      if (other.type < amber || other.type > AMPHIPOD_SIZE) {
        throw invalid_argument("invalid amphipod type to copy");
      }

      // cout << "this type is " << this->type << "; other type is " << other.type << endl;
      this->type = other.type;
    }

  public:
    // Initialization constructor for input text
    Amphipod(const char& c) {
      switch(c) {
        case 'A': { this->type = amber; break; }
        case 'B': { this->type = bronze; break; }
        case 'C': { this->type = copper; break; }
        case 'D': { this->type = desert; break; }
        default: throw invalid_argument("invalid amphipod character");
      }
    }

    // Default constructor; does not represent a valid amphipod
    Amphipod(): type(AMPHIPOD_SIZE) {}

    // Copy constructor
    Amphipod(const Amphipod& other);
    
    const AmphipodType GetType() const { return this->type; }
    const double EnergyPerStep() const { return pow(10.0, (double)(this->type)); }
    const char RepresentativeChar() const {
      switch(this->type) {
        case amber: return 'A';
        case bronze: return 'B';
        case copper: return 'C';
        case desert: return 'D';
        default: throw logic_error("amphipod cannot be represented; has invalid type");
      }
    }

    // Assignment operator
    Amphipod& operator=(const Amphipod& other) {
      // cout << "in amphipod assignment operator" << endl;
      if (this != &other) {
        this->copy(other);
      }
      return *this;
    }

    // Destructor
    ~Amphipod() {}
};

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const Amphipod& x) {
  os << x.RepresentativeChar();
  return os;
}

Amphipod::Amphipod(const Amphipod& other) {
  // cout << "this: " << this << ": copy constructor on " << &other << endl;
  this->copy(other);
}

class BurrowState {
  public:
    static const int ROOM_DEPTH = 2;

  private:
    vector<pair<Amphipod,bool>> hallway; // bool is true if space is occupied
    // maps the desired amphipod type for a room to what hallway index leads into the room, and what index the room is in "rooms"
    map<AmphipodType,pair<int,int>> type_hallway_room_indexes;
    vector<stack<Amphipod>> rooms;

    const vector<Amphipod> GetAmphipodsInRoom(const int& room_idx) const {
      vector<Amphipod> list;
      stack<Amphipod> room_copy(rooms[room_idx]);

      while (!room_copy.empty()) { // iterate through room
        Amphipod a(room_copy.top());
        room_copy.pop();

        list.push_back(a);
      }

      return list;
    }

    // whether fully or not
    const bool IsRoomCorrectlyOccupied(const int& room_idx) const {
      for (auto occupant:this->GetAmphipodsInRoom(room_idx)) {
        if (occupant.GetType() != static_cast<AmphipodType>(room_idx)) {
          return false;
        }
      }

      return true;
    }

    const vector<int> GetValidHallwayMoves(const int hallway_idx) const {
      vector<int> valid_hallway_positions;

      // Check to the left of the current position
      // cout << "Checking valid hallway moves to the left of " << hallway_idx << ": " << endl;
      for (int h = hallway_idx-1; h >= 0; --h) {
        // can't move past another amphipod in the hallway
        if (this->hallway[h].second) {
          break;
        }

        // can't stop on a space immediately outside any room
        bool h_outside_room = false;
        for (auto t:this->type_hallway_room_indexes) {
          if (h == t.second.first) {
            h_outside_room = true;
            break;
          }
        }
        // cout << h << " is outside of room? " << h_outside_room << endl;start
        if (h_outside_room) {
          continue;
        }

        valid_hallway_positions.push_back(h);
      }

      // Check to the right of the current position
      for (int h = hallway_idx+1; h < this->hallway.size(); ++h) {
        // can't move past another amphipod in the hallway
        if (this->hallway[h].second) {
          break;
        }

        // can't stop on a space immediately outside any room
        bool h_outside_room = false;
        for (auto t:this->type_hallway_room_indexes) {
          if (h == t.second.first) {
            h_outside_room = true;
            break;
          }
        }
        if (h_outside_room) {
          continue;
        }

        valid_hallway_positions.push_back(h);
      }

      return valid_hallway_positions;
    }

    // small helper function to hash this burrow's hallway into a comparable number
    // treat each hallway position as a digit in 5-nary counting system (4 amphipod states, one unoccupied state)
    const unsigned long long int HallwayHash() const {
      unsigned long long int hash = 0;

      for (double h = 0; h < this->hallway.size(); ++h) {
        pair<Amphipod,bool> a = this->hallway[h];
        if (a.second) {
          hash += ((unsigned long long int)(a.first.GetType()) + 1) * (unsigned long long int)pow((double)AMPHIPOD_SIZE+1, h);
        }
        // unocuppied hallway space equates to 0
      }

      return hash;
    }

    // small helper function to hash this burrow's rooms into a comparable number
    // treat each room position as a digit in 5-nary counting system (4 amphipod states, one unoccupied state)
    const unsigned long long int RoomHash() const {
      unsigned long long int hash = 0;
      double exp = 0;

      for (int r = 0; r < this->rooms.size(); ++r) {
        vector<Amphipod> room_contents = this->GetAmphipodsInRoom(r);
        for (int i = 0; i < BurrowState::ROOM_DEPTH; ++i) {
          if (i < room_contents.size()) {
            hash += ((unsigned long long int)(room_contents[i].GetType()) + 1) * (unsigned long long int)pow((double)AMPHIPOD_SIZE+1, exp);
          }
          // unocuppied room space equates to 0

          ++exp;
        }
      }

      return hash;
    }

    void clear() {
      // Clear hallway
      this->hallway.resize(0);

      // Clear type_hallway_room_indexes
      this->type_hallway_room_indexes.clear();

      // Clear rooms
      this->rooms.resize(0);
    }

    void copy(const BurrowState& other) {
      // Copy hallway
      this->hallway.resize(other.hallway.size());
      for (int h = 0; h < other.hallway.size(); ++h) {
        // cout << "copying an amphipod line 201 from " << &other << " from hallway position " << h << endl;
        this->hallway[h] = other.hallway[h];
      }

      // Copy type_hallway_room_indexes
      for (auto t:other.type_hallway_room_indexes) {
        this->type_hallway_room_indexes[t.first] = pair<int,int>(t.second.first, t.second.second);
      }

      // Copy rooms
      this->rooms.resize(other.rooms.size());
      for (int r = 0; r < other.rooms.size(); ++r) {
        stack<Amphipod> room_copy(other.rooms[r]);
        this->rooms[r] = room_copy;
      }
    }
    
  public:
    // Default constructor for easy map operator[] usage; does not produce valid state
    BurrowState() {}

    // Initialization constructor from input text
    BurrowState(const vector<string>& input_lines) {
      for (int line = input_lines.size()-1; line >= 0; --line) {
        int room_idx = 0;
        for (int line_idx = 0; line_idx < input_lines[line].size(); ++line_idx) {
          switch(input_lines[line][line_idx]) {
            case '.': {
              // cout << "creating hallway space from line index " << line_idx << endl;
              this->hallway.push_back(pair<Amphipod,bool>(Amphipod(), false));
              break;
            }
            case 'A':
            case 'B':
            case 'C':
            case 'D': {
              // cout << "creating amphipod in room " << room_idx << " from line index " << line_idx << ": " << input_lines[line][line_idx] << endl;
              Amphipod amphipod(input_lines[line][line_idx]);
              if (rooms.size() <= room_idx) {
                // cout << "creating room " << room_idx << " stack" << endl;
                rooms.push_back(stack<Amphipod>());
              }
              rooms[room_idx].push(amphipod);
              // cout << "room " << room_idx << " is now " << rooms[room_idx].size() << " elements" << endl;

              type_hallway_room_indexes[static_cast<AmphipodType>(room_idx)] = pair<int,int>(line_idx-1, room_idx); // relies on there being exactly 1 # before the hallway starts
              ++room_idx;
              break;
            }
          }
        }
      }
    }

    // true copy constructor
    BurrowState(const BurrowState& other) {
      this->copy(other);
    }

    // Return the BurrowState moving one of this's amphipods out of a room and into the hallway
    pair<BurrowState,int> CreateTransitionToHallway(const int& room_idx, const int& hallway_pos) const {
      // Perform initial copy
      BurrowState b(*this);

      // Move the amphipod from its room to the given hallway position
      Amphipod no_shortcut(b.rooms[room_idx].top());
      pair<Amphipod,bool> a(no_shortcut, true);
      b.rooms[room_idx].pop();
      b.hallway[hallway_pos] = a;

      // Calculate energy
      // move outside the room
      int spaces_moved = BurrowState::ROOM_DEPTH - b.rooms[room_idx].size();
      // move from outside the room to the hallway position
      for (auto t:b.type_hallway_room_indexes) {
        if (t.second.second == room_idx) {
          spaces_moved += (int)abs(hallway_pos - t.second.first);
          break;
        }
      }

      int energy_change = spaces_moved * a.first.EnergyPerStep();
      return pair<BurrowState,int>(b, energy_change);
    }

    // Return the BurrowState moving one of this's amphipods into its correct room from the hallway
    pair<BurrowState,int> CreateTransitionFromHallway(const int& hallway_idx) const {
      // Perform initial copy
      BurrowState b(*this);

      // Move the amphipod in the given hallway position to its correct room
      Amphipod a = b.hallway[hallway_idx].first;
      int correct_room_idx = b.type_hallway_room_indexes[a.GetType()].second;
      b.rooms[correct_room_idx].push(a);
      b.hallway[hallway_idx].second = false;

      // Calculate energy
      // move to the space outside the room
      int spaces_moved = (int)abs(hallway_idx - b.type_hallway_room_indexes[a.GetType()].first);
      // move into the room
      spaces_moved += BurrowState::ROOM_DEPTH - b.rooms[correct_room_idx].size() + 1;

      int energy_change = spaces_moved * a.EnergyPerStep();
      return pair<BurrowState,int>(b, energy_change);
    }

    bool operator<(const BurrowState& other) const { // requested by compiler since object is map key
      if (this->HallwayHash() < other.HallwayHash()) {
        return true;
      }
      else if (this->HallwayHash() == other.HallwayHash()) {
        return this->RoomHash() < other.RoomHash();
      }

      return false;
    }

    BurrowState& operator=(const BurrowState& other) { // implement assignment operator to work with priority queue
      if (this != &other) {
        this->clear();
        this->copy(other);
      }

      return *this;
    }

    // defined for ease of printing
    void Print(ostream& os) const {
      os << "Hallway hash: " << this->HallwayHash() << "; room hash: " << this->RoomHash() << endl;
      for (int line = 0; line <= BurrowState::ROOM_DEPTH; ++line) {
        for (int h = 0; h < this->hallway.size(); ++h) {
          // Print hallway itself
          if (line == 0) {
            if (this->hallway[h].second) {
              os << this->hallway[h].first << ' ';
            }
            else {
              os << ". ";
            }
          }
          // Print rooms below hallway
          else {
            bool room_printed = false;
            for (auto t:this->type_hallway_room_indexes) {
              if (t.second.first == h) {
                room_printed = true;
                vector<Amphipod> room_list = this->GetAmphipodsInRoom(t.second.second);
                if (line-1 < room_list.size()) {
                  os << room_list[line-1] << ' ';
                }
                else {
                  os << "  ";
                }
              }
            }

            if (!room_printed) {
              os << "  ";
            }
          }
        }
        os << endl;
      }
    }

    const double EnergyNeededHeuristic() const {
      double min_energy_needed = 0;

      // Check hallway
      for (double h = 0; h < this->hallway.size(); ++h) {
        pair<Amphipod,bool> a = this->hallway[h];
        if (a.second) {
          // cout << "line 349 type " << a.first.GetType() << endl;
          double distance_into_room = abs(h - this->type_hallway_room_indexes.at(a.first.GetType()).first) + 1.0;

          min_energy_needed += distance_into_room * a.first.EnergyPerStep();
        }
      }

      // Check rooms
      for (auto t:this->type_hallway_room_indexes) {
        AmphipodType correct_room_type = t.first;
        int this_room_hallway_idx = t.second.first;
        int this_room_idx = t.second.second;


        vector<Amphipod> room_list = this->GetAmphipodsInRoom(this_room_idx);
        for (auto a:room_list) {
          if (a.GetType() != correct_room_type) {
            // cout << "line 366 list of length " << room_list.size() << " has element with type " << a.GetType() << endl;
            double distance_into_correct_room = 1.0 + abs(this_room_hallway_idx - this->type_hallway_room_indexes.at(a.GetType()).first) + 1.0;

            min_energy_needed += distance_into_correct_room * a.EnergyPerStep();
          }
        }
      }

      return min_energy_needed;
    }

    const bool IsOrganized() const {
      // Check that hallway is empty
      for (auto a:hallway) {
        if (a.second) {
          return false;
        }
      }

      // Check that rooms are correctly filled
      for (auto t:this->type_hallway_room_indexes) {
        int this_room_idx = t.second.second;
        if (!this->IsRoomCorrectlyOccupied(this_room_idx)) {
          return false;
        }
      }

      return true;
    }

    vector<pair<BurrowState,int>> GetValidAdjacentStates() const {
      vector<pair<BurrowState,int>> adjacent_transitions;

      // See where each top amphipod on each room could move
      // Do not consider rooms that are already getting filled up with their correct occupants
      for (auto t:this->type_hallway_room_indexes) {
        if (!this->IsRoomCorrectlyOccupied(t.second.second) && this->rooms[t.second.second].size() > 0) {
          // cout << "Considering amphipod at the top of room " << t.second.second << endl;
          vector<int> new_positions = this->GetValidHallwayMoves(t.second.first);
          for (auto p:new_positions) {
            // cout << "creating neighbor: top amphipod can move from room " << t.second.second << " to hallway position " << p << endl;
            pair<BurrowState,int> new_state = this->CreateTransitionToHallway(t.second.second, p);
            adjacent_transitions.push_back(new_state);
          }
        }
      }

      // See where each amphipod in the hallway could move
      for (int h = 0; h < this->hallway.size(); ++h) {
        pair<Amphipod,bool> a = this->hallway[h];
        if (a.second) {
          // cout << "considering moving " << a.first << " in hallway position " << h << endl;

          // The room must contain only correct types before we can enter it
          int correct_room_idx = this->type_hallway_room_indexes.at(a.first.GetType()).second;
          bool all_correct = this->IsRoomCorrectlyOccupied(correct_room_idx);
          // cout << "all correct in the goal room index " << correct_room_idx << "? want " << true << ": " << all_correct << endl;

          // Ensure the hallway is empty on our way to our room
          int correct_room_entrance = this->type_hallway_room_indexes.at(a.first.GetType()).first;
          int path_min = h < correct_room_entrance ? h+1 : correct_room_entrance;
          int path_max = path_min == correct_room_entrance ? h-1 : correct_room_entrance;
          // cout << "checking path between " << path_min << " and " << path_max << " on way to room entrance " << correct_room_entrance << endl;

          bool path_empty = true; // until proven false
          for (int path_h = path_min; path_h <= path_max; ++path_h) {
            if (this->hallway[path_h].second) {
              // cout << "path blocked by " << this->hallway[path_h].first << " in index " << path_h << endl;
              path_empty = false;
              break;
            }
          }
          // cout << "path empty? want " << true << ": " << path_empty << endl;

          if (all_correct && path_empty) {
            // cout << "Creating neighbor; amphipod can move from hallway position " << h << " to its correct room" << endl;

            pair<BurrowState,int> new_state = this->CreateTransitionFromHallway(h);
            adjacent_transitions.push_back(new_state);
          }
        }
      }

      // cout << "returning " << adjacent_transitions.size() << " adjacent states" << endl;
      return adjacent_transitions;
    }

    ~BurrowState() {
      this->clear();
    }
};

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const BurrowState& x) {
  x.Print(os);
  return os;
}

class EnergyGuessForBurrowState {
  public:
    EnergyGuessForBurrowState(BurrowState b, int e): burrow_state(b), energy_guess(e) {}

    BurrowState burrow_state;
    int energy_guess;

    // assignment operator is required for this type to be used as element in a priority queue
    EnergyGuessForBurrowState& operator=(const EnergyGuessForBurrowState& other) {
      if (this == &other) {
        return *this;
      }

      this->burrow_state = other.burrow_state;
      this->energy_guess = other.energy_guess;
      return *this;
    }
};

// EnergyGuessComparison is used for burrow states being sorted in a priority queue by their energy cost guesses
class EnergyGuessComparison {
  public:
    bool operator() (const EnergyGuessForBurrowState& l, const EnergyGuessForBurrowState& r) const {
      return l.energy_guess > r.energy_guess;
    }
};

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day23/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  BurrowState start(file_results.first);

  // Part 1:
  // Follow A* algorithm, treating burrow states as nodes; developed referencing https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
  priority_queue<EnergyGuessForBurrowState, vector<EnergyGuessForBurrowState>, EnergyGuessComparison> state_queue;
  EnergyGuessForBurrowState start_guess(start, start.EnergyNeededHeuristic());
  state_queue.push(start_guess);
  // cout << "Start state:" << start << endl;

  // Create map tracking the directionality of the lowest energy path from start
  map<BurrowState,BurrowState> came_from;

  // Create map tracking the value of the lowest energy path from start to other states
  map<BurrowState,int> lowest_energy_needed;
  lowest_energy_needed[start] = 0;

  while (!state_queue.empty()) {
    // cout << "Taking another state off the top of priority queue with " << state_queue.size() << " states total in it" << endl;
    EnergyGuessForBurrowState current = state_queue.top();
    state_queue.pop();

    // cout << "Looking for a good move from:" << current.burrow_state << endl;

    // Check if we've reached the goal burrow state
    if (current.burrow_state.IsOrganized()) {
      cout << "Part 1 answer: " << lowest_energy_needed[current.burrow_state] << endl;
      break;
    }

    // Consider each move that the amphipods in the burrow could make to an adjacent state
    vector<pair<BurrowState,int>> transitions = current.burrow_state.GetValidAdjacentStates();
    // cout << transitions.size() << " valid transitions found" << endl;
    for (auto t:transitions) {
      int tentative_energy_needed = lowest_energy_needed[current.burrow_state] + t.second;
      // cout << "Potential next move: " << t.first << endl;

      if (lowest_energy_needed.find(t.first) == lowest_energy_needed.end() || tentative_energy_needed < lowest_energy_needed[t.first]) {
        // TODO Part 2: if same algorithm is not working to solve Part 2, it's possible that the heuristic still is not consistent; in solving Part 1, there were points where the lowest-energy-seen decreased (lowest_energy_needed.find(t.first) != lowest_energy_needed.end()), so if they were to be pulled off the queue a second time, they would have to be detected and squashed

        // cout << "Good next move: " << t.first << endl;
        // This move is the shortest-known path from the start through current
        came_from[t.first] = current.burrow_state;
        lowest_energy_needed[t.first] = tentative_energy_needed;

        EnergyGuessForBurrowState adjacent_guess(t.first, tentative_energy_needed + t.first.EnergyNeededHeuristic());
        state_queue.push(adjacent_guess);
      }
    }
    // cout << endl;
  }

  // Part 2:
  /* TODO ideas:
    because an amphipod will only move twice (once to the hallway, once to its room), it will only have 1 mystery point to go to -> 1 variable per amphipod in the burrow
    There are only a very small number of feasible stop options in the hallway
    every amphipod has a minimum movement = minimum energy to move to its final destination; so the solution is minimizing the additional steps outside of that path
    If Part 2 is some of the rules for movement change, I could encapsulate the edge-determination in a function & replace the function
  */

  return 0;
}
