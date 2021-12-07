// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 20 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: TODO
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>

using namespace std;

int main() {

  pair<vector<string>, int> file_results = ReadLinesFromFile("day20/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  for (auto line:file_results.first) {
    // TODO
  }

  // Part 1:

  // Part 2:
  // TODO

  return 0;
}
