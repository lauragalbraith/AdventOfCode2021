// main.cpp: Laura Galbraith
// Description: solver for Puzzle 2 of Day 2 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// What is the life support rating of the submarine?

#include <iostream>
#include <fstream>
#include <bitset> // would use https://www.cplusplus.com/reference/vector/vector-bool/ if binary length was unknown
#include <string>
#include <vector>
#include <queue>

using namespace std;

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

// O(TODO)
bitset<12> CalculateOxGenRating(queue<bitset<12>> values) {
  queue<bitset<12>>* initial_set = &values;
  int bit_under_consideration = 11; // start with the "first" position out of 12
  while (initial_set->size() > 1) {
    queue<bitset<12>>* ones_set = new queue<bitset<12>>;
    queue<bitset<12>>* zeros_set = new queue<bitset<12>>;

    // Determine how many of each bit value is seen
    for (int j = 0; j < initial_set->size()) {
      bitset<12> value = initial_set->pop_front();
      if (value[bit_under_consideration] == 1) {
        ones_set->push_back(value);
      }
      else {
        zeros_set->push_back(value);
      }
    }

    delete(initial_set);
    if (ones_set->size() > zeros_set->size()) {
      initial_set = ones_set;
      delete(zeros_set);
    }
    else {
      initial_set = zeros_set;
      delete(ones_set);
    }
  }

  bitset<12> final_value = initial_set->pop_front();
  delete(initial_set);

  return final_value;
}

int main() {
  // read in file, as vector of strings
  vector<string> input = ReadLinesFromFile("../input.txt");

  // parse lines from file as binary values
  queue<bitset<12>> initial_set();
  for (vector<string>::const_iterator i = input.begin(); i != input.end(); ++i) {
    bitset<12> binary_value(*i);
    initial_set.push_back(binary_value);
  }

  bitset<12> ox_gen_rating = CalculateOxGenRating(initial_set);

  cout << "Part 2:" << endl;
  cout << "Final oxygen generator rating is " << ox_gen_rating << " in binary and " << ox_gen_rating.to_ulong() << " in decimal" << endl;
  cout << "Final CO2 scrubber rating is " << "TODO" << " in binary and " << "TODO" << " in decimal" << endl;
  cout << "Answer: " << "TODO" << endl;

  return 0;
}
