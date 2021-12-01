// main.cpp: Laura Galbraith
// Description: solver for Puzzle 2 of Day 1 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Consider sums of a three-measurement sliding window. How many sums are larger than the previous sum?

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const string kInputFileName = "../input.txt";

// O(TODO once done) where n is the length of input
int BruteForce(const vector<int> input) {
  // TODO
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
