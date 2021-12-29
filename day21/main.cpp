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

using namespace std;

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

  public:
    Player(const int player_num, const int starting_space, const int win_min):
      number(player_num),
      min_win_score(win_min)
    {
      // cout << "regular constructor on " << this << ": num " << this->number << ", min win score " << this->min_win_score << endl;
      this->space = starting_space;
      this->score = 0;
      // TODO FINALLY remove debugging statements
    }

    Player(const Player& other): // copy constructor
      number(other.number),
      min_win_score(other.min_win_score)
    {
      // cout << "copy constructor on " << this << ": num " << this->number << ", min win score " << this->min_win_score << endl;

      this->space = other.space;
      this->score = other.score;
    }

    vector<Player*> TakeTurn(Die* die);
    const int GetPlayerNumber() const { return this->number; }
    const int GetScore() const { return this->score; }
    const bool HasWon() const {
      // cout << "HasWon has a score of " << this->score << " and a min win score of " << this->min_win_score << endl;
      return this->score >= this->min_win_score; 
    }

    ~Player() {}
};

// TakeTurn will return the resulting quantum players from the turn's actions
vector<Player*> Player::TakeTurn(Die* die) {
  queue<pair<Player*,vector<int>>> player_rolls;
  player_rolls.push(pair<Player*,vector<int>>(this, vector<int>()));

  vector<Player*> resulting_players;

  // Perform rolls in an expanding fashion
  while (!player_rolls.empty()) {
    pair<Player*,vector<int>> p = player_rolls.front();
    player_rolls.pop();
    Player* current_player = p.first;
    vector<int> rolls_so_far = p.second;

    // cout << current_player << " num " << current_player->GetPlayerNumber() << " has rolled " << rolls_so_far.size() << " times so far" << endl;

    // Check if all rolls for this player are done
    if (rolls_so_far.size() == 3) {
      int movement = rolls_so_far[0] + rolls_so_far[1] + rolls_so_far[2];

      // determine which space the player lands on
      int space_value = (current_player->space + movement) % 10;
      current_player->space = space_value == 0 ? 10 : space_value;

      // update score according to space
      current_player->score += current_player->space;
      // cout << current_player << " now has a score of " << current_player->score << endl;

      // include player in final result
      resulting_players.push_back(current_player);

      continue; // handle the next player in the queue
    }

    // Roll again for this player, potentially creating more player universes
    vector<int> quantum_rolls = die->Roll();
    for (int r = 0; r < quantum_rolls.size(); ++r) {
      // Add on this quantum roll to the rolls so far
      vector<int> total_rolls = vector<int>(rolls_so_far.begin(), rolls_so_far.end());
      total_rolls.push_back(quantum_rolls[r]);

      // Clone player for the new universe
      Player* player;
      if (r == 0) {
        player = current_player; // save space by keeping the original player record in use
      }
      else {
        // cout << "Calling copy constructor: " << current_player << ": num " << current_player->GetPlayerNumber() << " and min win score: " << current_player->min_win_score << endl;
        player = new Player(*current_player);
      }

      // Add player to the back of the queue to keep rolling
      player_rolls.push(pair<Player*,vector<int>>(player, total_rolls));
    }
  }

  return resulting_players;
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

  // Parse file input
  vector<Player*> dirac_players;
  for (auto line:file_results.first) {
    smatch m;
    regex_search(line, m, r);
    if (m.size() != 3) {
      cout << "Unexpected number of matches from regex on input line: " << line << ": " << m.size() << endl;
      return -1;
    }

    Player* player = new Player(stoi(m[1]), stoi(m[2]), 21);
    dirac_players.push_back(player);
  }

  if (dirac_players.size() != 2) {
    cout << "Unexpected number of players specified in file" << endl;
    return -1;
  }

  DiracDie* dirac_die = new DiracDie(3);

  queue<pair<Player*,Player*>> dirac_player_pairs;
  dirac_player_pairs.push(pair<Player*,Player*>(dirac_players[0], dirac_players[1]));

  map<int,unsigned long long int> wins;
  wins[dirac_players[0]->GetPlayerNumber()] = 0;
  wins[dirac_players[1]->GetPlayerNumber()] = 0;

  while (!dirac_player_pairs.empty()) {
    cout << "Universes: " << dirac_player_pairs.size() << endl;

    pair<Player*,Player*> players = dirac_player_pairs.front();
    dirac_player_pairs.pop();

    Player* turn_player = players.first;
    Player* next_player = players.second;

    // Check if this player's game has ended
    if (next_player->HasWon()) {
      // cout << next_player << " num " << next_player->GetPlayerNumber() << " has won with score of " << next_player->GetScore() << endl;

      wins[next_player->GetPlayerNumber()] += 1;
      delete turn_player;
      delete next_player;
      continue;
    }
    else if (turn_player->HasWon()) {
      // cout << turn_player << " num " << turn_player->GetPlayerNumber() << " has won with score of " << turn_player->GetScore() << endl;

      wins[turn_player->GetPlayerNumber()] += 1;
      delete turn_player;
      delete next_player;
      continue;
    }

    // Create new player universes based on this player's turn
    vector<Player*> resulting_turn_players = turn_player->TakeTurn(dirac_die);

    // Add all player pairs back to the queue, in their next turn order
    for (int p = 0; p < resulting_turn_players.size(); ++p) {
      // cout << "After their turn, " << p << ", num " << resulting_turn_players[p]->GetPlayerNumber() << " now has a score of " << resulting_turn_players[p]->GetScore() << endl;

      if (p == 0) { // save a little space by keeping the original next player pointer
        dirac_player_pairs.push(pair<Player*,Player*>(next_player, resulting_turn_players[p]));
      }
      else {
        dirac_player_pairs.push(pair<Player*,Player*>(new Player(*next_player), resulting_turn_players[p]));
      }
    }
  }

  cout << "Checking win counts" << endl;
  unsigned long long int max_win_count = 0;
  for (auto c:wins) {
    if (c.second > max_win_count) {
      max_win_count = c.second;
    }
  }

  cout << "Part 2 answer: " << max_win_count << endl; // TODO HERE process is killed before it finishes, which means I'm probably using too much memory; is there a way I could process each 27 universes until completion? I though maybe do player 1, then player 2, then store in a map their two scores/positions, which has a small defined number of values, MAPPED TO the number of universes with that player pair score/position state; Die method to return a vector of possible roll totals (ex. [3,4,4,4,...]); then for each of those movement numbers,  simulate player 1's turn then player 2's turn IN A GIANT LOOP over the map tracking all possible player states & how many universes are in that state

  // Clean up memory
  for (auto p:dirac_players) {
    delete p;
  }

  delete dirac_die;

  return 0;
}
