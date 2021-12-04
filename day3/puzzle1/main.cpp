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

int main() {
  // read in file, as vector of strings
  vector<string> input = ReadLinesFromFile("day3/input.txt");

  // parse lines from file as binary values
  vector<bitset<12>> values;
  for (vector<string>::const_iterator i = input.begin(); i != input.end(); ++i) {
    bitset<12> binary_value(*i);
    values.push_back(binary_value);
  }

  bitset<12> gamma_rate; // Each bit in the gamma rate can be determined by finding the most common bit in the corresponding position
  bitset<12> epsilon_rate; // The epsilon rate is calculated in a similar way; rather than use the most common bit, the least common bit from each position is used
  for (int i = 0; i < 12; ++i) {
    int ones_count = 0;
    int zeros_count = 0;

    // Determine how many of each bit value is seen
    // Are there any easy binary ops which can tell me the most common bit across an array of binary numbers? AND / OR / XOR / NOR - probably not, because binary operations take in two values, and if there was a whole pile of 0s vs one 0, it should interact with a single 1 the same... if we were to order them, maybe, but then we'd have to reorder them for every bit position
    for (vector<bitset<12>>::const_iterator j = values.begin(); j != values.end(); ++j) {
      bitset<12> value = *j;
      if (value[i] == 1) {
        ++ones_count;
      }
      else {
        ++zeros_count;
      }
    }

    gamma_rate[i] = 1 ? (ones_count > zeros_count) : 0; // if there are an equal number of 1s and 0s, 0 will be used (this is not specified in the puzzle rules, but has to be a choice somewhere)
    epsilon_rate[i] = ~gamma_rate[i]; // since there are only 2 bit options, the most common bit is the opposite of the least common bit
  }

  cout << "Part 1:" << endl;
  cout << "Final gamma rate is " << gamma_rate << " in binary and " << gamma_rate.to_ulong() << " in decimal" << endl;
  cout << "Final epsilon rate is " << epsilon_rate << " in binary and " << epsilon_rate.to_ulong() << " in decimal" << endl;
  cout << "Answer: " << gamma_rate.to_ulong() * epsilon_rate.to_ulong() << endl;

  return 0;
}
