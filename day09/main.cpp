// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 9 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the sum of the risk levels of all low points on your heightmap?
// Part 2: What do you get if you multiply together the sizes of the three largest basins?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>

using namespace std;

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day09/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  vector<vector<int>> height_map; // accessed by [row][col]
  for (auto line:file_results.first) {
    vector<int> line_heights;
    line_heights.resize(line.size());
    for (int i = 0; i < line.size(); ++i) {
      line_heights[i] = line[i] - '0'; // this evaluates to an int representing the number in the char
    }

    height_map.push_back(line_heights);
  }

  // Part 1:
  // Sum up risk levels
  int risk_sum = 0;
  for (int row = 0; row < height_map.size(); ++row) {
    for (int col = 0; col < height_map[row].size(); ++col) {
      int curr = height_map[row][col];
      if (row-1 >= 0 && curr >= height_map[row-1][col]) {
        continue;
      }
      if (row+1 < height_map.size() && curr >= height_map[row+1][col]) {
        continue;
      }
      if (col-1 >= 0 && curr >= height_map[row][col-1]) {
        continue;
      }
      if (col+1 < height_map[row].size() && curr >= height_map[row][col+1]) {
        continue;
      }

      risk_sum += curr + 1; // The risk level of a low point is 1 plus its height
    }
  }

  cout << "Part 1 answer: " << risk_sum << endl;

  // Part 2:
  // TODO
  cout << "Part 2 answer: " << "TODO" << endl;

  return 0;
}
