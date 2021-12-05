// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 4 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What will your final score be if you choose the first winning bingo board?
// Part 2: Once the last board wins, what would its final score be?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <tuple>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <regex>
#include <stdexcept> // invalid_argument

using namespace std;

class BingoBoard {
  private:
    class BingoCell { public: int val; bool marked; };
    vector<vector<BingoCell>> board_data; // ordered/accessed by [row][col]
    int last_number_marked;
    bool has_won;

  public:
    const static int kBingoNumRows = 5, kBingoNumColumns = 5;
    BingoBoard(vector<string> rows);
    void PrintBoardState(ostream &out);
    bool MarkNumber(int num);
    int FinalScore();
    bool HasWon();
};

// Constructor for BingoBoard, taking in board data in expected format from input
BingoBoard::BingoBoard(vector<string> rows) {
  // Check for valid format: number of rows
  if (rows.size() != BingoBoard::kBingoNumRows) {
    throw invalid_argument("invalid number of rows");
  }

  this->board_data = vector<vector<BingoCell>>();
  this->board_data.resize(BingoBoard::kBingoNumRows);
  for (int row = 0; row < rows.size(); ++row) {
    this->board_data[row] = vector<BingoCell>();
    this->board_data[row].resize(BingoBoard::kBingoNumColumns);
    int col = 0;

    string remaining_row_str = rows[row];
    smatch m;
    regex r(" *(\\d+)");
    while (regex_search(remaining_row_str, m, r) && col < BingoBoard::kBingoNumColumns) {
      if (m.size() != 2) {
        throw invalid_argument("invalid row format: '" + rows[row] + "'");
      }

      this->board_data[row][col].val = stoi(m[1], NULL, 10);
      this->board_data[row][col].marked = false;

      remaining_row_str = m.suffix().str();
      col++;
    }

    // Check for valid format: number of columns
    if (col != BingoBoard::kBingoNumColumns || remaining_row_str != "") {
      throw invalid_argument("invalid number of columns in row: '" + rows[row] + "'");
    }
  }

  // Initialize remaining members
  this->last_number_marked = -1;
  this->has_won = false;
}

// PrintBoardState sends the board state to the ostream, where the marked numbers are noted
// This was chosen instead of overloading operator<< to eliminate the need to expose the board data in public member functions
void BingoBoard::PrintBoardState(ostream &out) {
  out << "Won? " << (this->has_won ? "yes" : "no") << endl;

  for (vector<vector<BingoCell>>::const_iterator row_i = this->board_data.begin(); row_i != this->board_data.end(); ++row_i) {
    for (vector<BingoCell>::const_iterator col_i = (*row_i).begin(); col_i != (*row_i).end(); ++col_i) {
      if ((*col_i).marked) {
        out << "\033[3;32m"; // italic and green, because the bold isn't very visible
      }
      else {
        out << "\033[0;31m"; // red
      }

      out << setw(3);
      out << (*col_i).val; // 3 because we know our max value is 99
    }
    out << endl;
  }

  out << "\033[0;0m";
}

// Returns true if marking this number causes this board to win
// Since a bingo board is generally a bounded thing, and cannot be expected to grow to an unwieldy size, I will not worry about trying to optimize marking or win-checking
bool BingoBoard::MarkNumber(int num) {
  for (int row_i = 0; row_i < this->board_data.size(); ++row_i) {
    for (int col_i = 0; col_i < this->board_data[row_i].size(); ++col_i) {
      if (this->board_data[row_i][col_i].val == num) {
        // Mark the matching cell
        this->board_data[row_i][col_i].marked = true;
        this->last_number_marked = num;

        // Check for win: along row
        bool try_row_win = true;
        for (int check_col = 0; check_col < this->board_data[row_i].size(); ++check_col) {
          if (!(this->board_data[row_i][check_col].marked)) {
            try_row_win = false;
            break;
          }
        }

        if (try_row_win) {
          this->has_won = true;
          return this->has_won;
        }

        // Check for win: along column
        bool try_col_win = true;
        for (int check_row = 0; check_row < this->board_data.size(); ++check_row) {
          if (!(this->board_data[check_row][col_i].marked)) {
            try_col_win = false;
            break;
          }
        }

        if (try_col_win) {
          this->has_won = true;
          return this->has_won;
        }

        return false;
      }
    }
  }

  return false;
}

// Returns a number less than 0 if the board has not reached the win condition
int BingoBoard::FinalScore() {
  if (!this->has_won) {
    return -1;
  }

  int unmarked_sum = 0;
  for (vector<vector<BingoCell>>::const_iterator i = this->board_data.begin(); i != this->board_data.end(); ++i) {
    for (vector<BingoCell>::const_iterator j = (*i).begin(); j != (*i).end(); ++j) {
      if (!(*j).marked) {
        unmarked_sum += (*j).val;
      }
    }
  }

  return unmarked_sum * this->last_number_marked;
}

bool BingoBoard::HasWon() {
  return this->has_won;
}

int main() {
  pair<vector<string>, int> file_result = ReadLinesFromFile("day4/input.txt");
  if (file_result.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse bingo number list from file contents
  vector<int> bingo_numbers;
  string remaining_bingo_num_str = file_result.first[0];
  smatch m;
  regex r("(\\d+),");
  while (regex_search(remaining_bingo_num_str, m, r)) {
    if (m.size() != 2) {
      cout << "Unexpected bingo number format" << endl;
      return -1;
    }

    int bingo_num = stoi(m[1], NULL, 10);
    bingo_numbers.push_back(bingo_num);

    remaining_bingo_num_str = m.suffix().str();
  }

  // include the last bingo number, which is not followed by the separator
  int last_bingo_num = stoi(remaining_bingo_num_str, NULL, 10);
  bingo_numbers.push_back(last_bingo_num);

  // Parse bingo boards from remaining file contents
  vector<BingoBoard> boards;
  for (int i = 2; i < file_result.first.size(); i += BingoBoard::kBingoNumRows + 1) { // skipping bingo number line, newlines between boards
    vector<string> this_board_input;
    for (int j = i; j < i + BingoBoard::kBingoNumRows; ++j) {
      this_board_input.push_back(file_result.first[j]);
    }

    BingoBoard this_board(this_board_input);
    boards.push_back(this_board);
  }

  // Part 1: find first winning board
  int first_win_final_score = -1;
  int last_win_final_score = -1;
  // Simulate to determine winning bingo board
  for (vector<int>::const_iterator bingo_num = bingo_numbers.begin(); bingo_num != bingo_numbers.end(); ++bingo_num) {
    for (vector<BingoBoard>::iterator i = boards.begin(); i != boards.end(); ++i) {
      if ((*i).HasWon()) {
        continue; // for our purposes, we don't care about this board anymore
      }

      bool board_has_won = (*i).MarkNumber(*bingo_num);
      if (board_has_won) {
        int final_score = (*i).FinalScore();
        if (first_win_final_score < 0) {
          first_win_final_score = final_score;
          cout << "Part 1 Answer: " << first_win_final_score << endl;
        }

        last_win_final_score = final_score;
      }
    }
  }

  // Part 2: last winning board
  cout << "Part 2 Answer: " << last_win_final_score << endl;

  return 0;
}
