// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 11 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: Given the starting energy levels of the dumbo octopuses in your cavern, simulate 100 steps. How many total flashes are there after 100 steps?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <queue>

using namespace std;

// ValidAdjacentCoordinates returns a list of adjacent <row,col> pairs that are adjacent to the given row and col values
// Considers row=rowLimit invalid value (rowLimit-1 would be valid)
vector<pair<int,int>> ValidAdjacentCoordinates(const int row, const int col, const int rowLimit, const int colLimit) {
  vector<pair<int,int>> coords;
  for (int r = row-1; r <= row+1; ++r) {
    for (int c = col-1; c <= col+1; ++c) {
      if (r == row && c == col) {
        continue; // the original coordinate is not adjacent to itself
      }

      if (r >= 0 && r < rowLimit && c >= 0 && c < colLimit) {
        coords.push_back(pair<int,int>(r,c));
      }
    }
  }

  return coords;
}

int PerformStep(vector<vector<int>>& octopus_energies) {
  queue<pair<int,int>> flashed_octopuses;
  int flashes_during_step = 0;

  // Increment all octopuses once, adding flashing ones to the queue to process
  for (int row = 0; row < octopus_energies.size(); ++row) {
    for (int col = 0; col < octopus_energies[row].size(); ++col) {
      int curr = octopus_energies[row][col];
      octopus_energies[row][col] += 1;

      // Check if the octopus flashes
      if (curr == 9) {
        ++flashes_during_step;
        for (auto a:ValidAdjacentCoordinates(row, col, octopus_energies.size(), octopus_energies[row].size())) {
          flashed_octopuses.push(a);
        }
      }
    }
  }

  // Process all octopus flashes by incrementing adjacent octopuses
  while (!flashed_octopuses.empty()) {
    pair<int,int> o = flashed_octopuses.front();
    flashed_octopuses.pop();

    int curr = octopus_energies[o.first][o.second];
    octopus_energies[o.first][o.second] += 1;

    // Check if this octopus now flashes
    if (curr == 9) {
      ++flashes_during_step;
      for (auto a:ValidAdjacentCoordinates(o.first, o.second, octopus_energies.size(), octopus_energies[o.first].size())) {
        flashed_octopuses.push(a);
      }
    }
  }

  // Reset octopuses that have flashed to an energy level of 0
  for (int row = 0; row < octopus_energies.size(); ++row) {
    for (int col = 0; col < octopus_energies[row].size(); ++col) {
      if (octopus_energies[row][col] >= 10) {
        octopus_energies[row][col] = 0;
      }
    }
  }

  return flashes_during_step;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day11/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  vector<vector<int>> octopus_energies; // accessed by [row][col]
  for (auto line:file_results.first) {
    vector<int> energy_line;
    energy_line.resize(line.size());
    for (int i = 0; i < line.size(); ++i) {
      energy_line[i] = line[i] - '0'; // this evaluates to an int representing the number in the char
    }
    octopus_energies.push_back(energy_line);
  }

  // Part 1:
  unsigned long long int flash_count = 0;
  int first_all_flash_step = -1;

  int step = 1;
  for (; step <= 100; ++step) {
    int flashes_during_step = PerformStep(octopus_energies);

    // Count up if all octopuses flashed simultaneously
    if (first_all_flash_step < 0 && flashes_during_step == octopus_energies.size() * octopus_energies[0].size()) {
      first_all_flash_step = step;
    }
    flash_count += flashes_during_step;
  }

  cout << "Part 1 answer: " << flash_count << endl;

  for (; first_all_flash_step < 0 ; ++step) {
    int flashes_during_step = PerformStep(octopus_energies);
    // Count up if all octopuses flashed simultaneously
    if (flashes_during_step == octopus_energies.size() * octopus_energies[0].size()) {
      break;
    }
  }

  cout << "Part 2 answer: " << step << endl;

  return 0;
}

// TODO ideas for future days: determine what version of C++ my compiler is using
