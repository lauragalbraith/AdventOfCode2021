// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 21 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What do you get if you multiply the score of the losing player by the number of times the die was rolled during the game?
// Part 2: Find the player that wins in more universes; in how many universes does that player win?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>
#include <queue>
#include <map>
#include <cmath>

using namespace std;

const int NUM_ROLLS = 3;

class Die {
  protected:
    int times_rolled;

  public:
    Die() { this->times_rolled = 0; }

    virtual const vector<int> Roll() {
      ++this->times_rolled;
      return vector<int>();
    }
    int GetTimesRolled() const { return this->times_rolled; }

    virtual ~Die() {}
};

class DeterministicDie : public Die {
  private:
    int last_roll;
    const unsigned int sides;

  public:
    DeterministicDie(const unsigned int die_sides):
      Die(),
      sides(die_sides)
    {
      this->last_roll = die_sides;
    }

    const vector<int> Roll() {
      // Call base class method
      Die::Roll();

      // Roll to next number, circling back from 100 to 1
      this->last_roll = this->last_roll == this->sides ? 1 : this->last_roll + 1;

      return vector<int>(1, this->last_roll);
    }

    ~DeterministicDie() {}
};

class DiracDie : public Die {
  private:
    const unsigned int sides;
    vector<int> rolls;

  public:
    DiracDie(const unsigned int die_sides):
      Die(),
      sides(die_sides)
    {
      this->rolls.resize(die_sides);
      for (int r = 1; r <= die_sides; ++r) {
        this->rolls[r-1] = r;
      }
    }

    // returns the roll options in ascending order
    const vector<int> Roll() {
      Die::Roll();
      return this->rolls;
    }

    ~DiracDie() {}
};

class Player {
  private:
    const int number;
    int space;
    const int min_win_score;
    int score;

    void MoveAndUpdateScore(const int movement);

  public:
    Player(const int player_num, const int starting_space, const int win_min):
      number(player_num),
      min_win_score(win_min)
    {
      this->space = starting_space;
      this->score = 0;
    }

    Player(const Player& other): // copy constructor
      number(other.number),
      min_win_score(other.min_win_score)
    {
      this->space = other.space;
      this->score = other.score;
    }

    vector<Player*> TakeTurn(Die* die);
    const int GetPlayerNumber() const { return this->number; }
    const int GetScore() const { return this->score; }
    const int GetSpace() const { return this->space; }
    const bool HasWon() const {
      return this->score >= this->min_win_score; 
    }

    // returns based on number, score, and space on board
    // Because we want players with the smallest scores to be handled first (because scores cannot decrease), that will be the strongest factor to consider, then potential to increase score
    bool operator<(const Player& other) const { // necessary to implement for to use Player type as key in map
      if (this->score < other.score) {
        return true;
      }
      else if (this->score == other.score) {
        if (this->space < other.space) {
          return true;
        }
        else if (this->space == other.space) {
          if (this->number < other.number) {
            return true;
          }
        }
      }

      return false;
    }

    ~Player() {}
};

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const Player& p) {
  os << "Player " << p.GetPlayerNumber() << " has score " << p.GetScore() << "; is on space " << p.GetSpace();
  return os;
}

// TakeTurn will return the resulting quantum players from the turn's actions
vector<Player*> Player::TakeTurn(Die* die) {
  // Determine what the movement options for the player are
  vector<double> all_movements;
  vector<vector<int>> roll_options;
  int option_count = 1;
  for (int rolls = 1; rolls <= NUM_ROLLS; ++rolls) {
    roll_options.push_back(die->Roll());
    option_count *= roll_options[roll_options.size()-1].size();
  }
  all_movements.resize(option_count);

  // Each movement option is a combination of roll options, where the movement's identifier (mov_idx) is equal to o0 * 3^2 + o1 * 3^1 + o2 * 3^0 where o0 is the *index* of the roll option chosen for the 3rd roll (example where 3 is the number of options per roll, and the exponents follow the range of indexes into the rolls (i.e. 0-2 for 3 rolls))
  // NOTE: this would not work if we wanted to split universes with some identical roll options (ex. 1,1,2)
  for (double mov_idx = 0; mov_idx < all_movements.size(); ++mov_idx) {
    double movement = 0;
    double remaining_mov_idx = mov_idx;

    for (double roll_idx = NUM_ROLLS-1; roll_idx >= 0; --roll_idx) {
      for (double option_idx = roll_options[roll_idx].size()-1; option_idx >= 0; --option_idx) {
        double potential_option = option_idx * pow(roll_options[roll_idx].size(), roll_idx);
        if (remaining_mov_idx - potential_option >= 0) {
          movement += roll_options[roll_idx][option_idx];
          remaining_mov_idx -= potential_option;
          break; // stop considering options; move to next roll
        }
      }
    }

    all_movements[mov_idx] = movement;
  }

  // Create a new player universe out of any >1 movement options
  vector<Player*> resulting_players;
  resulting_players.resize(all_movements.size());
  for (double m = 0; m < all_movements.size(); ++m) {
    Player* current_player;
    if (m == 0) {
      current_player = this;
    }
    else {
      current_player = new Player(*this);
    }
    resulting_players[m] = current_player;
  }

  // once all player copies have been made, update the original's, and all others', scores
  for (double m = 0; m < resulting_players.size(); ++m) {
    resulting_players[m]->MoveAndUpdateScore((int)all_movements[m]);
  }

  return resulting_players;
}

void Player::MoveAndUpdateScore(const int movement) {
  // determine which space the player lands on
  int space_value = (this->space + movement) % 10;
  this->space = space_value == 0 ? 10 : space_value;

  // update score according to space
  this->score += this->space;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day21/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Part 1: players have a different win condition

  // Parse file input
  vector<Player*> practice_players;
  regex r("^Player (\\d+) starting position: (\\d+)$");
  for (auto line:file_results.first) {
    smatch m;
    regex_search(line, m, r);
    if (m.size() != 3) {
      cout << "Unexpected number of matches from regex on input line: " << line << ": " << m.size() << endl;
      return -1;
    }

    Player* player = new Player(stoi(m[1]), stoi(m[2]), 1000);
    practice_players.push_back(player);
  }

  if (practice_players.size() != 2) {
    cout << "Unexpected number of players specified in file" << endl;
    return -1;
  }

  DeterministicDie* det_die = new DeterministicDie(100);

  int winning_player_idx = -1;
  int turn_player_idx = 0; // start with player 1 (the first in the list)
  while (winning_player_idx < 0) {
    // take the current player's turn
    vector<Player*> resulting_players = practice_players[turn_player_idx]->TakeTurn(det_die);
    if (resulting_players.size() != 1 || resulting_players[0] != practice_players[turn_player_idx]) {
      cout << "Practice turn not handled as expected; " << resulting_players.size() << " resulting players were returned" << endl;
      return -1;
    }

    // Check if the player has won
    if (practice_players[turn_player_idx]->HasWon()) {
      winning_player_idx = turn_player_idx;
      break;
    }

    // move to next player
    turn_player_idx = (turn_player_idx+1) % practice_players.size();
  }

  Player* losing_player = winning_player_idx == 0 ? practice_players[1] : practice_players[0];
  cout << "Part 1 answer: " << losing_player->GetScore() * det_die->GetTimesRolled() << endl;

  // Clean up memory
  for (auto p:practice_players) {
    delete p;
  }

  delete det_die;

  // Part 2:

  // Re-parse file input
  vector<Player> dirac_players;
  for (auto line:file_results.first) {
    smatch m;
    regex_search(line, m, r);
    if (m.size() != 3) {
      cout << "Unexpected number of matches from regex on input line: " << line << ": " << m.size() << endl;
      return -1;
    }

    Player p(stoi(m[1]), stoi(m[2]), 21);
    dirac_players.push_back(p);
  }

  if (dirac_players.size() != 2) {
    cout << "Unexpected number of players specified in file" << endl;
    return -1;
  }

  DiracDie* dirac_die = new DiracDie(3);

  // Track the number of wins of each player
  map<int,unsigned long long int> wins;
  wins[dirac_players[0].GetPlayerNumber()] = 0;
  wins[dirac_players[1].GetPlayerNumber()] = 0;

  // Since players with the same number, score, and occupied space on the board are equivalently-likely to win, track pairs of players at any two values the same
  // The pair type's evaluation does not use our operator< with pointer types, so for simplicity, we will not use Player pointers
  map<pair<Player,Player>, unsigned long long int> universes;
  pair<Player,Player> starting(dirac_players[0], dirac_players[1]);
  universes[starting] = 1;

  while (universes.size() > 0) {
    auto u = universes.begin();

    // Clear the universes from the record while processing
    Player player1 = u->first.first;
    Player player2 = u->first.second;
    unsigned long long int universe_count = u->second;

    universes.erase(u);

    // Since wins must be checked before the universe is expanded next, check only after universes have been expanded from previous player's turn
    // Before the 1st player's turn, check 2nd player's wins now
    if (player2.HasWon()) {
      wins[player2.GetPlayerNumber()] += universe_count;
      continue;
    }

    // Take the 1st player's turn
    vector<Player*> resulting_player1s = player1.TakeTurn(dirac_die);

    // Before the 2nd player's turn, check if 1st player has won
    bool index0_still_stack_pointer = true;
    for (int i = resulting_player1s.size()-1; i >= 0; --i) { // move backwards so we don't skip any values while erasing
      if (resulting_player1s[i]->HasWon()) {
        wins[resulting_player1s[i]->GetPlayerNumber()] += universe_count;

        // If the first player1 in the list is no longer on the stack, we will need to delete it later
        if (i == 0) {
          index0_still_stack_pointer = false;
        }
        else {
          delete resulting_player1s[i];
        }

        resulting_player1s.erase(resulting_player1s.begin()+i);
      }
    }

    // Take the 2nd player's turn
    vector<Player*> resulting_player2s = player2.TakeTurn(dirac_die);

    // Combine the resulting players' universes to save in the map
    for (auto p1:resulting_player1s) {
      for (auto p2:resulting_player2s) {
        pair<Player,Player> new_universe(*p1,*p2);
        if (universes.find(new_universe) == universes.end()) {
          universes[new_universe] = 0;
        }
        universes[new_universe] += universe_count;
      }
    }

    // Clear memory used in resulting player pointers (skip index 0 only if it's a stack pointer)
    for (int i = index0_still_stack_pointer ? 1 : 0; i < resulting_player1s.size(); ++i) {
      delete resulting_player1s[i];
    }
    for (int i = 1; i < resulting_player2s.size(); ++i) {
      delete resulting_player2s[i];
    }
  }

  unsigned long long int max_win_count = 0;
  for (auto c:wins) {
    if (c.second > max_win_count) {
      max_win_count = c.second;
    }
  }

  cout << "Part 2 answer: " << max_win_count << endl;

  // Clean up memory
  delete dirac_die;

  return 0;
}
