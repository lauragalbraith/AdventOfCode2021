// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 15 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the lowest total risk of any path from the top left to the bottom right?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>

using namespace std;

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day15/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  for (auto line:file_results.first) {
    // TODO
  }

  // Part 1:
  // TODO IDEA: path-finding algorithm (probably Dijkstra's) where distance is jsut the value of the end node, and adjacent nodes just being up,down,left,right

  // Part 2:
  // TODO

  return 0;
}
