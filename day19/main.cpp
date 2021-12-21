// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 19 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: How many beacons are there?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile,ParseSeparatedInts
#include <iostream>
#include <tuple>
#include <vector>
#include <string>

using namespace std;

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day19/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  for (auto line:file_results.first) {
    // TODO use std::pair<std::vector<int>, int> ParseSeparatedInts(const std::string str, const std::string sep)
  }

  // Part 1:
  // TODO notes:
  //  scanner data is relative to the scanner, not the entire 3D region
  //  overlapping detection region is one where "there are at least 12 beacons that both scanners detect within the overlap"
  //  "scanners also don't know their rotation or facing direction" - this means +x could be -z, etc. (24 possible orientations of a scanner)
  // TODO ideas:
  //  could I instead orient the code relative to the first beacon of the first scanner?
  //  make a const for 1000 as the max detection radius of a scanner
  //  24 is small enough that I could rotate all of a single scanner's points to check if they (relative to the first beacon) were mapped the same as any of the next scanner's in line?
  // Set full_space: (the scanner-set to match against) equal to scanner 0
  // While the list of remaining unmatched scanners is not empty: (FIFO queue?)
  //   Set to_match: take the scanner at the top of list
  //   For each possible x/y/z rotation:
  //     For each beacon A in to_match:
  //       For each beacon B in full_space:
  //         Move to_match so A is on top of B
  //         Count the number of matching beacons
  //         If the count >= 12:
  //           Add all beacons/scanner into full_space, at the current A-B rotation
  //           continue;
  //   Add to_match back to the rear of the list and continue to top of while loop

  cout << "Part 1 answer: " << "TODO number of distinct beacons in full_space" << endl;

  // Part 2:
  // TODO

  return 0;
}
