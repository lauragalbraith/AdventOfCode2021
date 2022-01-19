// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 25 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the first step on which no sea cucumbers move?
// Part 2: Christmas!

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

// lower number represents higher priority
enum MovementType { east=0, south=1, MOVEMENT_TYPE_LIMIT };

// Abstract base class for sea cucumbers
class SeaCucumber {
  public:
    // Constructors
    SeaCucumber() {}

    // Functional methods
    virtual MovementType GetMovementType() const { return MOVEMENT_TYPE_LIMIT; }
    virtual const char GetChar() const { return '*'; }

    // must be made virtual so that we can delete any object using the derived class's destructor
    // made default so this method becomes a pure virtual method, and the class becomes abstract
    virtual ~SeaCucumber() = default;
};

class EastMovingSeaCucumber : public SeaCucumber {
  public:
    // Constructors
    EastMovingSeaCucumber() {}

    MovementType GetMovementType() const { return east; }
    const char GetChar() const { return '>'; }

    virtual ~EastMovingSeaCucumber() {}
};

class SouthMovingSeaCucumber : public SeaCucumber {
  public:
    // Constructors
    SouthMovingSeaCucumber() {}

    MovementType GetMovementType() const { return south; }
    const char GetChar() const { return 'v'; }

    virtual ~SouthMovingSeaCucumber() {}
};

class SeaFloorState {
  private:
    vector<vector<SeaCucumber*>> state; // accessed like [row][col]

    // returns result in form (row,col)
    pair<int,int> SpaceAhead(const int& row, const int& col, const MovementType& move_type) {
      int row_ahead = row;
      int col_ahead = col;

      switch(move_type) {
        case east: {
          ++col_ahead;
          if (col_ahead == this->state[row].size()) {
            col_ahead = 0;
          }
          break;
        }
        case south: {
          ++row_ahead;
          if (row_ahead == this->state.size()) {
            row_ahead = 0;
          }
          break;
        }

        default: throw invalid_argument("invalid movement type to move a space ahead");
      }

      return pair<int,int>(row_ahead,col_ahead);
    }

    void clear() {
      for (auto row:this->state) {
        for (auto position:row) {
          delete position;
        }
        row.resize(0);
      }
      this->state.resize(0);
    }

  public:
    // Constructor which takes in the file input format
    SeaFloorState(const vector<string>& input) {
      int num_rows = input.size();
      int num_cols = input[0].size();
      
      this->state.resize(num_rows);
      for (int row = 0; row < num_rows; ++row) {
        if (input[row].size() != num_cols) {
          throw invalid_argument("inconsistent number of columns in input row");
        }
        
        this->state[row].resize(num_cols);
        for (int col = 0; col < num_cols; ++col) {
          SeaCucumber* c;
          switch(input[row][col]) {
            case '.': c = NULL; break;
            case '>': c = new EastMovingSeaCucumber(); break;
            case 'v': c = new SouthMovingSeaCucumber(); break;

            default: throw invalid_argument("invalid sea cucumber character");
          }

          this->state[row][col] = c;
        }
      }
    }

    // returns the number of cucumbers that moved during the step
    int PerformStep() {
      int cucumbers_moved = 0;

      // initialize next floor state to be empty
      vector<vector<SeaCucumber*>> next_state;
      next_state.resize(this->state.size());
      for (int row = 0; row < this->state.size(); ++row) {
        next_state[row].resize(this->state[row].size());
        for (int col = 0; col < this->state[row].size(); ++col) {
          next_state[row][col] = NULL;
        }
      }

      // update all positions in the next state according to the current state of the floor
      // for each type of sea cucumber, move all of its type and only store state in the next state
      // each cucumber will make sure that there's nothing in front of it before and during the step before it moves
      // (this is to account for south-movers after east-movers have transitioned)
      for (int pri = 0; pri < MOVEMENT_TYPE_LIMIT; ++pri) {
        // determine where each sea cucumber will be in the next state
        for (int row = 0; row < this->state.size(); ++row) {
          for (int col = 0; col < this->state[row].size(); ++col) {
            SeaCucumber* c = this->state[row][col];

            if (c != NULL && c->GetMovementType() == pri) {
              pair<int,int> space_ahead = SpaceAhead(row, col, c->GetMovementType());

              // move only occurs if space ahead is empty (consider both cucumbers that have already moved, and cucumbers that have yet to move)
              // because cucumbers can only move down and to the right (the same directions in which we're iterating row/col), this is a valid check
              if (this->state[space_ahead.first][space_ahead.second] == NULL && next_state[space_ahead.first][space_ahead.second] == NULL) {
                next_state[space_ahead.first][space_ahead.second] = c;
                ++cucumbers_moved;
              }
              else {
                next_state[row][col] = c;
              }
            }
          }
        }

        // now that all cucumbers of one type have moved, remove them from tracking of the initial state
        // (in the middle of the pri-movement, they need to be there to block both adjacent and wrap-around cucumbers from thinking they're safe to move)
        // at the end of the movement, they need to be in only one place to accurately reflect where the next movers can go
        if (pri < MOVEMENT_TYPE_LIMIT-1) { // we can skip this if the last movers have now moved
          for (int row = 0; row < this->state.size(); ++row) {
            for (int col = 0; col < this->state[row].size(); ++col) {
              SeaCucumber* c = this->state[row][col];
              if (c != NULL && c->GetMovementType() == pri) {
                // remove cucumber from the current state's tracking; it was copied to next-state's tracking above
                this->state[row][col] = NULL;
              }
            }
          }
        }
      }

      // update internal state of this floor object
      // what was previously in state is totally preserved in the next state, so no memory clearing needs to happen here
      this->state = next_state;

      return cucumbers_moved;
    }

    int NumberRows() const { return this->state.size(); }
    int NumberColumns() const {
      if (this->state.size() > 0) {
        return this->state[0].size();
      }
      else {
        return 0;
      }
    }

    const char GetSeaCucumberCharAt(const int& row, const int& col) const {
      if (row < this->state.size() && col < this->state[row].size()) {
        SeaCucumber* c = this->state[row][col];
        if (c == NULL) {
          return '.';
        }
        else {
          return c->GetChar();
        }
      }

      return '-';
    }

    // Deconstructor to clean up memory used internally
    ~SeaFloorState() {
      this->clear();
    }
};

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const SeaFloorState& s) {
  os << "Sea floor state:" << endl;

  for (int row = 0; row < s.NumberRows(); ++row) {
    for (int col = 0; col < s.NumberColumns(); ++col) {
      os << s.GetSeaCucumberCharAt(row, col);
    }
    os << endl;
  }

  return os;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day25/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  SeaFloorState* current_state = new SeaFloorState(file_results.first);

  // Part 1:
  // Perform steps until no cucumbers are moving
  int step = 0;
  int cucumbers_moved = 0;
  do {
    ++step;
    cucumbers_moved = current_state->PerformStep();
  } while (cucumbers_moved != 0);

  cout << "Part 1 answer: " << step << endl;

  // Clean up memory
  delete current_state;

  return 0;
}
