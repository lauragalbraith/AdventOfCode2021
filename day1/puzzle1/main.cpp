// main.cpp: Laura Galbraith
// Description: solver for Puzzle 1 of Day 1 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// How many measurements are larger than the previous measurement?

#include "../../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

using namespace std;

const string kInputFileName = "day1/input.txt";

// O(n) where n is the length of input
int BruteForce(const vector<int> input) {
  if (input.size() <= 1) {
    return 0;
  }

  int count = 0;
  vector<int>::const_iterator prev, curr;
  prev = input.begin();
  for (curr = prev + 1; curr != input.end(); prev++, curr++) {
    if (*curr > *prev) { // "larger" == strictly increasing
      ++count;
    }
  }
  return count;
}

int main() {
  // Read in file
  pair<vector<string>, int> file_result = ReadLinesFromFile(kInputFileName);
  if (file_result.second < 0) {
    cout << "Unable to read file" << endl;
    return -1;
  }

  vector<int> input;
  for (vector<string>::const_iterator i = file_result.first.begin(); i != file_result.first.end(); ++i) {
    int x = stoi(*i);
    input.push_back(x);
  }

  // Process input
  int answer = BruteForce(input);
  cout << answer << endl;

  return 0;
}
