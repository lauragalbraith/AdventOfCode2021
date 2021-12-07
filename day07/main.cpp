// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 7 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: The crab movement cost is constant; How much fuel must they spend to align to that position?
// Part 2: The crab movement cost increases linearly; How much fuel must they spend to align to that position?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>
#include <cmath>
#include <algorithm>

using namespace std;

int main() {

  pair<vector<string>, int> file_results = ReadLinesFromFile("day07/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  if (file_results.first.size() != 1) {
    cout << "Unexpected file length" << endl;
    return -1;
  }

  vector<int> crab_positions;
  string remaining_file_str = file_results.first[0];
  smatch m;
  regex r("(\\d+),");
  while (regex_search(remaining_file_str, m, r)) {
    if (m.size() != 2) {
      cout << "Unexpected file format" << endl;
      return -1;
    }

    int c = stoi(m[1]);
    crab_positions.push_back(c);
    remaining_file_str = m.suffix().str();
  }
  // get last fish number
  int last_crab = stoi(remaining_file_str);
  crab_positions.push_back(last_crab);

  double crab_sum = 0;
  for (auto crab:crab_positions) {
    crab_sum += 1.0 * crab;
  }
  double crab_avg = crab_sum / (1.0 * crab_positions.size());
  double low_crab_avg = floor(crab_avg);
  double high_crab_avg = ceil(crab_avg);
  cout << "Average " << crab_avg << " floored and ceilinged: " << low_crab_avg << " " << high_crab_avg << endl;


  // Part 1:
  // Could take brute-force and check each of these options and keep track of the minimum fuel cost found
  // The answer is the median of the data, which is the "mid-point" that's easiest overall to move to
  sort(crab_positions.begin(), crab_positions.end()); // O(n * log(n))

  int low_med = crab_positions[crab_positions.size() / 2], high_med = crab_positions[crab_positions.size() / 2 + 1];
  cout << "Median: " << low_med << " and the next highest possiblity: " << high_med << endl;

  // Options for aligning the crabs are in the range of min(crab_positions) and max(crab_positions)
  cout << "Range of crabs: " << crab_positions[0] << " to " << crab_positions[crab_positions.size() - 1] << endl;

  // Calculate the fuel cost around the median values
  double min_fuel_cost = -1;
  int min_fuel_cost_point = -1;
  for (int i = low_med; i < high_med; ++i) {
    // Calculate fuel cost at this chosen point
    double fuel_cost = 0;
    for (auto crab:crab_positions) {
      fuel_cost += abs(1.0 * i - crab);
    }

    if (min_fuel_cost == -1 || fuel_cost < min_fuel_cost) {
      min_fuel_cost = fuel_cost;
      min_fuel_cost_point = i;
    }
  }

  cout << "Part 1 answer: " << min_fuel_cost << " at point " << min_fuel_cost_point << endl;

  // Part 2:
  // Crab movement cost is 1 to move 1 space to the chosen point, 2 to move the next space, etc.
  // The answer point works out to be the average because it's a weighted mid-point
  // Try both low and high average values to see which one would have the lower fuel cost
  unsigned long long int fuel_cost_from_low_avg = 0, fuel_cost_from_high_avg = 0;
  for (auto crab:crab_positions) {
    double spaces_to_move = abs(low_crab_avg - 1.0 * crab);
    fuel_cost_from_low_avg += spaces_to_move * (spaces_to_move + 1) / 2; // sum of natural numbers up to spaces_to_move

    spaces_to_move = abs(high_crab_avg - 1.0 * crab);
    fuel_cost_from_high_avg += spaces_to_move * (spaces_to_move + 1) / 2; // sum of natural numbers up to spaces_to_move
  }

  cout << "Fuel cost from average floored and ceilinged: " << fuel_cost_from_low_avg << " " << fuel_cost_from_high_avg << endl;
  cout << "Part 2 answer: " << (fuel_cost_from_low_avg < fuel_cost_from_high_avg ? fuel_cost_from_low_avg : fuel_cost_from_high_avg) << endl;

  return 0;
}
