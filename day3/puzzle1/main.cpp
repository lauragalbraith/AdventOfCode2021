// main.cpp: Laura Galbraith
// Description: solver for Puzzle 1 of Day 2 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// What is the power consumption of the submarine?

#include <iostream>
#include <fstream>
#include <bitset> // would use https://www.cplusplus.com/reference/vector/vector-bool/ if binary length was unknown
#include <string>
#include <vector>

using namespace std;

// TODO figure out how to make this function accessible by every puzzle since many use it
// O(n) where n is the number of lines in the file
const vector<string> ReadLinesFromFile(const string file_name) {
  ifstream f(file_name);
  vector<string> lines;
  if (f.is_open()) {
    string line;
    while (getline(f, line)) {
      lines.push_back(line);
    }
  }
  else {
    cout << "File could not be opened" << endl;
  }
  return lines;
}

const vector<bitset> 

int main() {
  // read in file, as vector of strings
  vector<string> input = ReadLinesFromFile("../input.txt");
  // investigate C++ types for binary numbers, because I want to examine bit by bit

  // Are there any easy binary ops which can tell me the most common bit across an array of binary numbers? AND / OR / XOR / NOR - probably not, because binary operations take in two values, and if there was a whole pile of 0s vs one 0, it should interact with a single 1 the same... if we were to order them, maybe, but then we'd have to reorder them for every bit position
  return 0;
}
