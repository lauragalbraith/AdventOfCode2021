// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 14 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What do you get if you take the quantity of the most common element and subtract the quantity of the least common element?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>
#include <queue>
#include <map>

using namespace std;

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day14/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  string polymer = file_results.first[0];
  vector<pair<string,string>> pair_insertion_rules;
  regex pair_insertion_rgx("([A-Z]{2}) -> ([A-Z])");
  for (int i = 2; i < file_results.first.size(); ++i) {
    smatch m;
    regex_search(file_results.first[i], m, pair_insertion_rgx);
    if (m.size() != 3) {
      cout << "Unexpected line format: " << file_results.first[i] << endl;
    }

    pair_insertion_rules.push_back(pair<string,string>(m[1], m[2]));
  }

  // Part 1:
  // Apply 10 steps of pair insertion to the polymer template
  for (int step = 1; step <= 10; ++step) {
    // cout << "step " << step << endl;

    // In order to insert new elements without altering where remaining elements have to be inserted, order than from back to front by insertion index
    // int indexes will be used rather than string::iterator types, because after any insert() call, any iterator value previously set will be invalid
    priority_queue<int> insertion_indexes;
    map<int,string> elements_to_insert;

    // Detect all insertion candidates
    for (auto rule:pair_insertion_rules) {
      size_t found = polymer.find(rule.first, 0);
      while (found != string::npos) {
        // cout << rule.first << " found at " << found << endl;

        insertion_indexes.push(found);
        if (elements_to_insert.find(found) != elements_to_insert.end()) {
          // No two insertion indexes should be equal, because no two pair insertion clauses are equal
          cout << "More than one element is due to be inserted at " << found + 1 << "; this is unexpected" << endl;
          return -1;
        }

        elements_to_insert[found] = rule.second;

        found = polymer.find(rule.first, found + 1);
      }
    }

    // Perform insertions
    while (insertion_indexes.size() > 0) {
      int idx = insertion_indexes.top();
      insertion_indexes.pop();

      polymer.insert(idx + 1, elements_to_insert[idx]);
    }

    // cout << polymer << endl; // TODO FINALLY remove debug statements
  }

  // Use a map to count how many times each element appears in the result
  map<char,int> element_count;
  for (auto c:polymer) {
    if (element_count.find(c) == element_count.end()) {
      element_count[c] = 0;
    }
    element_count[c] += 1;
  }

  // Check what the smallest number and largest number are
  int min_count = -1, max_count = -1;
  for (auto elem:element_count) {
    if (min_count < 0 || elem.second < min_count) {
      min_count = elem.second;
    }
    if (elem.second > max_count) {
      max_count = elem.second;
    }
  }

  cout << "Part 1 answer: " << max_count - min_count << endl;

  // Part 2:
  // TODO

  return 0;
}
