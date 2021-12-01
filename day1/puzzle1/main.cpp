// main.cpp: Laura Galbraith
// Description: solver for Puzzle 1 of Day 1 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// How many measurements are larger than the previous measurement?

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const string kInputFileName = "input.txt";

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
  ifstream input_file(kInputFileName);
  vector<int> input;
  if (input_file.is_open()) {
    string line;
    while (getline(input_file, line)) {
      int x = stoi(line);
      input.push_back(x);
    }
  }
  else {
    cout << "Unable to open file" << endl;
    return -1;
  }

  // Process input
  int answer = BruteForce(input);
  cout << answer << endl;

  return 0;
}
