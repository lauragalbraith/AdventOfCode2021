// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 6 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: How many lanternfish would there be after 80 days?
// Part 2: How many lanternfish would there be after 256 days?

#include "../util/fileutil.hpp" // ReadLinesFromFile, ParseSeparatedInts
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>

using namespace std;

int main() {

  pair<vector<string>, int> file_results = ReadLinesFromFile("day06/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  pair<vector<int>,int> fish_results = ParseSeparatedInts(file_results.first[0], ",");
  if (fish_results.second < 0) {
    cout << "Unexpected file format" << endl;
    return -1;
  }

  // Part 1:
  for (int day = 1; day <= 80; ++day) {
    int days_starting_fish_count = fish_results.first.size();
    for (int f = 0; f < days_starting_fish_count; ++f) {
      int until_repro = fish_results.first[f];
      if (until_repro == 0) {
        fish_results.first[f] = 6;
        fish_results.first.push_back(8);
      }
      else {
        fish_results.first[f] = until_repro - 1;
      }
    }
  }

  cout << "Part 1 answer: " << fish_results.first.size() << endl;

  // Part 2: after 256 days, there would be too many to store and/or too slow to compute. Thus, the strategy will change to tracking the fish a different way; instead keep track of how many fish have a certain int value

  // Initialize this tracking
  vector<unsigned long long int> fish_with_age; // indexed by age
  fish_with_age.resize(9);
  for (int i = 0; i <= 8; ++i) {
    fish_with_age[i] = 0;
  }
  for (auto f:fish_results.first) {
    fish_with_age[f] += 1;
  }

  for (int day = 81; day <= 256; ++day) {
    unsigned long long int fish_starting_at_0 = fish_with_age[0]; // save this value so it's not overwritten

    for (int new_fish_age = 0; new_fish_age < 8; ++new_fish_age) {
      fish_with_age[new_fish_age] = fish_with_age[new_fish_age + 1];
    }

    fish_with_age[6] += fish_starting_at_0; // renew fish who reach 0
    fish_with_age[8] = fish_starting_at_0; // copy fish who reach 0
  }

  unsigned long long int total_fish = 0;
  for (auto f:fish_with_age) {
    total_fish += f;
  }

  cout << "Part 2 answer: " << total_fish << endl;

  return 0;
}
