// main.cpp: Laura Galbraith
// Description: solver for Puzzle 2 of Day 2 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// What is the life support rating of the submarine?

#include "../../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <fstream>
#include <bitset> // would use https://www.cplusplus.com/reference/vector/vector-bool/ if binary length was unknown
#include <string>
#include <tuple>
#include <vector>
#include <queue>

using namespace std;

// O(n) where n is the number of lines in the file
/*const vector<string> ReadLinesFromFile(const string file_name) { // TODO remove once I get the header working
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
}*/

// O(x*n) runtime where x is the number of bit positions in the data (in our case, 12), and n is the initial number of binary values (which never decreases in the worst case)
// O(x*n) memory used, because the whole list is only stored in one place in each iteration, though it is duplicated in the intiial memory allocation
bitset<12> CalculateOxGenRating(queue<bitset<12>> values) {
  queue<bitset<12>>* initial_set = new queue<bitset<12>>(values); // use "new" rather than just taking the pointer of "values" so it is initialized on the heap
  int bit_under_consideration = 11; // start with the "first" position out of 12
  while (initial_set->size() > 1 && bit_under_consideration >= 0) {
    // Determine how many of each bit value is seen
    queue<bitset<12>>* ones_set = new queue<bitset<12>>;
    queue<bitset<12>>* zeros_set = new queue<bitset<12>>;

    int elems_left = initial_set->size();
    for (int i = 0; i < elems_left; ++i) {
      bitset<12> value = initial_set->front();
      initial_set->pop();
      if (value[bit_under_consideration] == 1) {
        ones_set->push(value);
      }
      else {
        zeros_set->push(value);
      }
    }

    delete initial_set;
    if (ones_set->size() >= zeros_set->size()) { // now specified: If 0 and 1 are equally common, keep values with a 1 in the position being considered
      initial_set = ones_set;
      delete zeros_set;
    }
    else {
      initial_set = zeros_set;
      delete ones_set;
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

// O(x*n) runtime where x is the number of bit positions in the data (in our case, 12), and n is the initial number of binary values (which never decreases in the worst case)
// O(x*n) memory used, because the whole list is only stored in one place in each iteration, though it is duplicated in the intiial memory allocation
bitset<12> CalculateCO2ScrubberRating(queue<bitset<12>> values) {
  queue<bitset<12>>* initial_set = new queue<bitset<12>>(values); // use "new" rather than just taking the pointer of "values" so it is initialized on the heap
  int bit_under_consideration = 11; // start with the "first" position out of 12
  while (initial_set->size() > 1 && bit_under_consideration >= 0) {
    // Determine how many of each bit value is seen
    queue<bitset<12>>* ones_set = new queue<bitset<12>>;
    queue<bitset<12>>* zeros_set = new queue<bitset<12>>;

    int elems_left = initial_set->size();
    for (int i = 0; i < elems_left; ++i) {
      bitset<12> value = initial_set->front();
      initial_set->pop();
      if (value[bit_under_consideration] == 1) {
        ones_set->push(value);
      }
      else {
        zeros_set->push(value);
      }
    }

    delete initial_set;
    // now specified: If 0 and 1 are equally common, keep values with a 0 in the position being considered
    if (zeros_set->size() <= ones_set->size()) { // TODO if changing this single line from > to < gets me the right answer, pass a function into this function to be > or < to get rid of duplicate code
      initial_set = zeros_set;
      delete ones_set;
    }
    else {
      initial_set = ones_set;
      delete zeros_set;
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
  pair<vector<string>, int> file_result = ReadLinesFromFile("../input.txt");
  if (file_result.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // parse lines from file as binary values
  vector<string> input = file_result.first;
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

  /*
   C++ Language Options
           -fabi-version=n  -fno-access-control -faligned-new=n  -fargs-in-order=n  -fchar8_t  -fcheck-new -fconstexpr-depth=n  -fconstexpr-loop-limit=n -fconstexpr-ops-limit=n -fno-elide-constructors
           -fno-enforce-eh-specs -fno-gnu-keywords -fno-implicit-templates -fno-implicit-inline-templates -fno-implement-inlines  -fms-extensions -fnew-inheriting-ctors -fnew-ttp-matching -fno-nonansi-builtins
           -fnothrow-opt  -fno-operator-names -fno-optional-diags  -fpermissive -fno-pretty-templates -frepo  -fno-rtti  -fsized-deallocation -ftemplate-backtrace-limit=n -ftemplate-depth=n
           -fno-threadsafe-statics  -fuse-cxa-atexit -fno-weak  -nostdinc++ -fvisibility-inlines-hidden -fvisibility-ms-compat -fext-numeric-literals -Wabi=n  -Wabi-tag  -Wconversion-null  -Wctor-dtor-privacy
           -Wdelete-non-virtual-dtor  -Wdeprecated-copy  -Wdeprecated-copy-dtor -Wliteral-suffix -Wmultiple-inheritance  -Wno-init-list-lifetime -Wnamespaces  -Wnarrowing -Wpessimizing-move  -Wredundant-move
           -Wnoexcept  -Wnoexcept-type  -Wclass-memaccess -Wnon-virtual-dtor  -Wreorder  -Wregister -Weffc++  -Wstrict-null-sentinel  -Wtemplates -Wno-non-template-friend  -Wold-style-cast -Woverloaded-virtual
           -Wno-pmf-conversions -Wno-class-conversion  -Wno-terminate -Wsign-promo  -Wvirtual-inheritance
  */

  return 0;
}
