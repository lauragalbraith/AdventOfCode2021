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

// O(m*n) where n is the length of input, and m is kWindowSize
// Since kWindowSize=3 is small, it is trivial to re-compute each sum it every time. 
int BruteForce(const vector<int> input) {
  if (input.size() <= kWindowSize) {
    return 0;
  }

  int increases = 0;
  for (int prev = 0, curr = 1; curr < input.size() - kWindowSize + 1; ++prev, ++curr) {
    // Compute the sums of each window
    int prev_sum = 0, curr_sum = 0;
    // Compute the sum in the window using the constant variable rather than having to assume its value is 3 here
    for (int i = prev; i < prev + kWindowSize; ++i) {
      prev_sum += input[i];
    }
    for (int i = curr; i < curr + kWindowSize; ++i) {
      curr_sum += input[i];
    }
    
    // Evaluate if it increased
    if (curr_sum > prev_sum) { 
      ++increases;
    }
  }
  return increases;
}

// O(n) where n is the length of input; m (where m is window_size) elements are looped through, doing constant work, and then n-m elements are looped through, doing constant work
// If the window size became large, we should cache each sum to only compute it once, or have a sliding computation which subtracts the dropped window element and adds the new window element
int SlidingSums(const vector<int> input, const int window_size) {
  if (input.size() <= window_size) {
    return 0;
  }

  int increases = 0;
  int prev_sum = 0, curr_sum = 0;
  // Compute first sum
  for (int i = 0; i < window_size; ++i) {
    prev_sum += input[i];
  }

  // Compute the new sum in a rolling fashion
  for (int prev_idx = 0, incoming_win_idx = window_size; incoming_win_idx < input.size(); ++prev_idx, ++incoming_win_idx) {
    curr_sum = prev_sum - input[prev_idx] + input[incoming_win_idx];
    if (curr_sum > prev_sum) {
      ++increases;
    }

    prev_sum = curr_sum;
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
  int answer = SlidingSums(input, kWindowSize);
  cout << answer << endl;

  return 0;
}
