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
  queue<bitset<12>>* initial_set = new queue<bitset<12>>(values); // use "new" rather than just taking the pointer of "values" so it is initialized on the heap
  cout << "entered CalculateOxGenRating with " << initial_set->size() << " elements" << endl;
  int bit_under_consideration = 11; // start with the "first" position out of 12
  while (initial_set->size() > 1 && bit_under_consideration >= 0) {
    // cout << "in while loop with " << endl;
    // cout << initial_set->size() << endl;
    // cout << bit_under_consideration << endl;
    // Determine how many of each bit value is seen
    queue<bitset<12>>* ones_set = new queue<bitset<12>>;
    queue<bitset<12>>* zeros_set = new queue<bitset<12>>;

    int elems_left = initial_set->size();
    for (int i = 0; i < elems_left; ++i) {
      // cout << "in for loop with " << endl;
      // cout << i << endl;
      // cout << initial_set->size() << endl;
      bitset<12> value = initial_set->front();
      initial_set->pop();
      if (value[bit_under_consideration] == 1) {
        ones_set->push(value);
      }
      else {
        zeros_set->push(value);
      }

      // cout << "end of for loop" << endl;
    }
    // cout << "outside of for loop with" << endl;
    // cout << initial_set->size() << endl;
    // cout << ones_set->size() << endl;
    // cout << zeros_set->size() << endl;

    delete initial_set;
    // cout << "after cleaning initial_set" << endl;
    if (ones_set->size() >= zeros_set->size()) { // now specified: If 0 and 1 are equally common, keep values with a 1 in the position being considered
      // cout << "before cleaning zeros_set" << endl;
      initial_set = ones_set;
      delete zeros_set;
      // cout << "after cleaning zeros_set" << endl; // TODO remove printout comments
    }
    else {
      // cout << "before cleaning ones_set" << endl;
      initial_set = zeros_set;
      delete ones_set;
      // cout << "after cleaning ones_set" << endl;
    }

    --bit_under_consideration;
  }

  if (initial_set->size() != 1) {
    cout << "error: algorithm resulted in non-terminal set of elements" << endl;
  }

  bitset<12> final_value = initial_set->front();
  // not bothering to pop that last element, as we are cleaning up the memory immediately after checking the element
  delete initial_set;

  return final_value;
}

// O(TODO)
bitset<12> CalculateCO2ScrubberRating(queue<bitset<12>> values) { // TODO run program thru valgrind/asan/msan/tsan to see if I missed any memory allocations
  queue<bitset<12>>* initial_set = new queue<bitset<12>>(values); // use "new" rather than just taking the pointer of "values" so it is initialized on the heap
  cout << "entered CalculateCO2ScrubberRating with " << initial_set->size() << " elements" << endl;
  int bit_under_consideration = 11; // start with the "first" position out of 12
  while (initial_set->size() > 1 && bit_under_consideration >= 0) {
    // cout << "in while loop with " << endl;
    // cout << initial_set->size() << endl;
    // cout << bit_under_consideration << endl;
    // Determine how many of each bit value is seen
    queue<bitset<12>>* ones_set = new queue<bitset<12>>;
    queue<bitset<12>>* zeros_set = new queue<bitset<12>>;

    int elems_left = initial_set->size();
    for (int i = 0; i < elems_left; ++i) {
      // cout << "in for loop with " << endl;
      // cout << i << endl;
      // cout << initial_set->size() << endl;
      bitset<12> value = initial_set->front();
      initial_set->pop();
      if (value[bit_under_consideration] == 1) {
        ones_set->push(value);
      }
      else {
        zeros_set->push(value);
      }

      // cout << "end of for loop" << endl;
    }
    // cout << "outside of for loop with" << endl;
    // cout << initial_set->size() << endl;
    // cout << ones_set->size() << endl;
    // cout << zeros_set->size() << endl;

    delete initial_set;
    // cout << "after cleaning initial_set" << endl;
    // now specified: If 0 and 1 are equally common, keep values with a 0 in the position being considered
    if (zeros_set->size() <= ones_set->size()) { // TODO if changing this single line from > to < gets me the right answer, pass a function into this function to be > or < to get rid of duplicate code
      // cout << "before cleaning ones_set" << endl;
      initial_set = zeros_set;
      delete ones_set;
      // cout << "after cleaning ones_set" << endl;
    }
    else {
      // cout << "before cleaning zeros_set" << endl;
      initial_set = ones_set;
      delete zeros_set;
      // cout << "after cleaning zeros_set" << endl;
    }

    --bit_under_consideration;
  }

  if (initial_set->size() != 1) {
    cout << "error: algorithm resulted in non-terminal set of elements" << endl;
  }

  bitset<12> final_value = initial_set->front();
  // not bothering to pop that last element, as we are cleaning up the memory immediately after checking the element
  delete initial_set;

  return final_value;
}

int main() {
  // read in file, as vector of strings
  vector<string> input = ReadLinesFromFile("../input.txt");

  // parse lines from file as binary values
  queue<bitset<12>> initial_set;
  for (vector<string>::const_iterator i = input.begin(); i != input.end(); ++i) {
    bitset<12> binary_value(*i);
    initial_set.push(binary_value);
  }

  bitset<12> ox_gen_rating = CalculateOxGenRating(initial_set);
  bitset<12> co2_scrubber_rating = CalculateCO2ScrubberRating(initial_set);

  cout << "Part 2:" << endl;
  cout << "Final oxygen generator rating is " << ox_gen_rating << " in binary and " << ox_gen_rating.to_ulong() << " in decimal" << endl;
  cout << "Final CO2 scrubber rating is " << co2_scrubber_rating << " in binary and " << co2_scrubber_rating.to_ulong() << " in decimal" << endl;
  cout << "Answer: " << ox_gen_rating.to_ulong() * co2_scrubber_rating.to_ulong() << endl;

  return 0;
}
