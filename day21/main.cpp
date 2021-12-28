// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 21 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What do you get if you multiply the score of the losing player by the number of times the die was rolled during the game?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>

using namespace std;

class Die {
  protected:
    int times_rolled;

  public:
    Die() { this->times_rolled = 0; }

    virtual int Roll() {
      ++this->times_rolled;
      return -1;
    }
    int GetTimesRolled() const { return this->times_rolled; }

    virtual ~Die() {}
};

class DeterministicDie : public Die {
  private:
    int last_roll;
    unsigned int sides;

  public:
    DeterministicDie(const unsigned int die_sides): Die() {
      this->last_roll = die_sides;
      this->sides = die_sides;
    }

    int Roll() {
      // Call base class method
      Die::Roll();

      // Roll to next number, circling back from 100 to 1
      this->last_roll = this->last_roll == this->sides ? 1 : this->last_roll + 1;

      return this->last_roll;
    }

    ~DeterministicDie() {}
};

class Player {
  private:
    const int number;
    int space;
    int score;

  public:
    Player(const int player_num, const int starting_space): number(player_num) {
      this->space = starting_space;
      this->score = 0;
      // TODO FINALLY remove debugging statements
    }

    bool TakeTurn(Die* die);
    const int GetScore() const { return this->score; }

    ~Player() {}
};

// TakeTurn will return true if this player has now won from this turn's actions
bool Player::TakeTurn(Die* die) {
  // Roll dice three times
  int movement = 0;
  for (int roll = 1; roll <= 3; ++roll) {
    movement += die->Roll();
  }

  // Determine what space we landed on
  int space_value = (this->space + movement) % 10;
  this->space = space_value == 0 ? 10 : space_value;

  // Update score with the value of the space landed on
  this->score += this->space;

  // Return based on whether we've won or not
  return this->score >= 1000;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day21/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  vector<Player*> players;
  regex r("^Player (\\d+) starting position: (\\d+)$");
  for (auto line:file_results.first) {
    smatch m;
    regex_search(line, m, r);
    if (m.size() != 3) {
      cout << "Unexpected number of matches from regex on input line: " << line << ": " << m.size() << endl;
      return -1;
    }

    Player* player = new Player(stoi(m[1]), stoi(m[2]));
    players.push_back(player);
  }

  // Part 1:
  DeterministicDie* det_die = new DeterministicDie(100);

  int winning_player_idx = -1;
  int turn_player_idx = 0; // start with player 1 (the first in the list)
  while (winning_player_idx < 0) {
    // take the current player's turn
    bool has_won = players[turn_player_idx]->TakeTurn(det_die);
    if (has_won) { winning_player_idx = turn_player_idx; }

    // move to next player
    turn_player_idx = (turn_player_idx+1) % players.size();
  }

  Player* losing_player = winning_player_idx == 0 ? players[1] : players[0];

  cout << "Part 1 answer: " << losing_player->GetScore() * det_die->GetTimesRolled() << endl;

  // Part 2:
  // TODO

  // Clean up memory
  for (auto p:players) {
    delete p;
  }

  delete det_die;

  return 0;
}
