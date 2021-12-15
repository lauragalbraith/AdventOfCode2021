// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 14 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: After 10 steps: what do you get if you take the quantity of the most common element and subtract the quantity of the least common element?
// Part 2: After 40 steps: what do you get if you take the quantity of the most common element and subtract the quantity of the least common element?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>
#include <queue>
#include <map>

using namespace std;

void PerformStep(string& polymer, const vector<pair<string,string>>& pair_insertion_rules) {
  // In order to insert new elements without altering where remaining elements have to be inserted, order than from back to front by insertion index
  // int indexes will be used rather than string::iterator types, because after any insert() call, any iterator value previously set will be invalid
  priority_queue<int> insertion_indexes;
  map<int,string> elements_to_insert;

  // Detect all insertion candidates
  for (auto rule:pair_insertion_rules) {
    size_t found = polymer.find(rule.first, 0);
    while (found != string::npos) {
      insertion_indexes.push(found);
      if (elements_to_insert.find(found) != elements_to_insert.end()) {
        // No two insertion indexes should be equal, because no two pair insertion clauses are equal
        cout << "More than one element is due to be inserted at " << found + 1 << "; this is unexpected" << endl;
        return;
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
}

unsigned long long int LargestCountMinusSmallest(const map<char,unsigned long long int>& element_count) {
  // Check what the smallest number and largest number are
  unsigned long long int min_count = 0, max_count = 0;
  for (auto elem:element_count) {
    if (min_count == 0 || elem.second < min_count) {
      min_count = elem.second;
    }
    if (elem.second > max_count) {
      max_count = elem.second;
    }
  }

  return max_count - min_count;
}

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

  // the start and the end characters of the polymer will always remain the same (insertions always occur in the middle of a pair of characters)
  char start = polymer[0], end = polymer[polymer.size()-1];

  // Part 1:
  // Apply 10 steps of pair insertion to the polymer template
  for (int step = 1; step <= 10; ++step) {
    PerformStep(polymer, pair_insertion_rules);
  }

  // Use a map to count how many times each element appears in the result
  map<char,unsigned long long int> element_count;
  for (auto c:polymer) {
    if (element_count.find(c) == element_count.end()) {
      element_count[c] = 0;
    }
    element_count[c] += 1;
  }

  cout << "Part 1 answer: " << LargestCountMinusSmallest(element_count) << endl;

  // Part 2:
  // Doing it directly takes too long, so rather than forming the string directly, we will keep track of all pairs of letters and how many times they occur
  // For example:
  //  "NNCB" would be "NN"=1,"NC"=1,"CB"=1
  //  applying a single step with rules NN -> C,NC -> B,CB -> H
  //  "NC"=1,"CN"=1,"NN"=0,"NB"=1,"BC"=1,"CH"=1,"HB"=1,"CB"=0; with equivalent string NCNBCHB

  // Initialize our new tracking system
  map<string,unsigned long long int> pair_count;
  for (int i = 0; i < polymer.size() - 1; ++i) {
    string p = polymer.substr(i, 2);
    if (pair_count.find(p) == pair_count.end()) {
      pair_count[p] = 0;
    }
    pair_count[p] += 1;
  }

  // Apply 40 steps of pair insertion to the polymer template
  for (int step = 11; step <= 40; ++step) {
    // Calculate diff in letter pairs
    map<string,long long int> pair_count_diffs;
    // for each rule: "rule.first[0]:rule.second" and "rule.second:rule.first[1]" increases by num("rule.first") while "rule.first" decreases by num("rule.first") (remembering to apply all rules simultaneously)
    for (auto rule:pair_insertion_rules) {
      // only process rule if the rule's letter pair is in the string
      if (pair_count.find(rule.first) != pair_count.end()) {
        // ... the first letter in the rule pair combined with the inserted element
        string front_gaining_pair = rule.first.substr(0,1) + rule.second;
        if (pair_count_diffs.find(front_gaining_pair) == pair_count_diffs.end()) {
          pair_count_diffs[front_gaining_pair] = 0;
        }

        pair_count_diffs[front_gaining_pair] += pair_count[rule.first];

        // ... the inserted element combined with the second letter in the rule pair
        string back_gaining_pair = rule.second + rule.first.substr(1,1);
        if (pair_count_diffs.find(back_gaining_pair) == pair_count_diffs.end()) {
          pair_count_diffs[back_gaining_pair] = 0;
        }

        pair_count_diffs[back_gaining_pair] += pair_count[rule.first];

        // ... the original rule pair
        pair_count_diffs[rule.first] -= pair_count[rule.first];
      }
    }

    // Apply diff to tracking status
    for (auto diff:pair_count_diffs) {
      if (pair_count.find(diff.first) == pair_count.end()) {
        if (diff.second < 0) {
          cout << diff.first << " unexpectedly is decreasing count when we have not seen it yet" << endl;
          return -1;
        }

        pair_count[diff.first] = 0;
      }

      pair_count[diff.first] += diff.second;
    }
  }

  // Use a map to count how many times each element appears in the result
  map<char,unsigned long long int> element_count2;
  for (auto p:pair_count) {
    for (auto c:p.first) {
      if (element_count2.find(c) == element_count2.end()) {
        element_count2[c] = 0;
      }
      element_count2[c] += p.second;
    }
  }

  // Because they are being counted from pairs, and all letters but start/end occur in at least 2 pairs, we will adjust at the end
  for (auto e:element_count2) {
    if (e.first == start || e.first == end) {
      element_count2[e.first] = (element_count2[e.first] + 1) / 2;
    }
    else {
      element_count2[e.first] = element_count2[e.first] / 2;
    }
  }

  cout << "Part 2 answer: " << LargestCountMinusSmallest(element_count2) << endl;

  return 0;
}

// TODO this problem may be solveable by memoization
