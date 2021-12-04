// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 4 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// What will your final score be if you choose the first winning bingo board?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <regex>

using namespace std;

/*
Ideas:
use classes to represent a bingo board, with operations on it like checking for a win, or perhaps marking a number returns a boolean on whether it won or not, operation to determine final score
Since a bingo board is generally a bounded thing, and cannot be expected to grow to an unwieldy size, I will not worry about trying to optimize marking or win-checking
use vector of vectors for bingo board representation
*/

const int kBingoNumRows = 5;
const int kBingoNumColumns = 5;

int main() {
  pair<vector<string>, int> file_result = ReadLinesFromFile("day4/input.txt"); // TODO HERE this fails when running from the root dir - will need to change other directories to get this to work everywhere; could replace the input text file name in make... with grep :)
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
  for (int i = 2; i < file_result.first.size(); i += kBingoNumRows + 1) { // skipping bingo number line, newlines between boards
    // TODO
  }

  // Part 1:
  // Simulate to determine winning bingo board
  // TODO

  // Print final score for winning bingo board
  // TODO

  // Part 2:
  // TODO when it is revealed

  return 0;
}
