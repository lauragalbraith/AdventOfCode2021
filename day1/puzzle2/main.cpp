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
const int kWindowSize = 3;

// O(TODO once done) where n is the length of input
int BruteForce(const vector<int> input) {
  if (input.size() <= kWindowSize) {
    return 0;
  }

  int increases = 0;
  for (int prev = 0, curr = 1; curr < input.size() - kWindowSize + 1; ++prev, ++curr) {
    cout << input[prev] << " " << input[curr] << endl;
  }
  return increases;
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
