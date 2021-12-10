// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 10 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: Find the first illegal character in each corrupted line of the navigation subsystem. What is the total syntax error score for those errors?
// Part 2: Find the completion string for each incomplete line, score the completion strings, and sort the scores. What is the middle score?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <stdexcept>

using namespace std;

class ChunkRelations {
  private:
    map<char,pair<char,int>> relations;
  public:
    ChunkRelations();
    bool IsCharacterOpening(const char& c);
    bool IsCharacterClosing(const char& c);
    bool DoesCharacterBCloseA(const char& a, const char& b);
    int Score(const char& c);
};

ChunkRelations::ChunkRelations() {
  this->relations['('] = pair<char,int>(')', 3);
  this->relations['['] = pair<char,int>(']',57);
  this->relations['{'] = pair<char,int>('}',1197);
  this->relations['<'] = pair<char,int>('>',25137);
}

bool ChunkRelations::IsCharacterOpening(const char& c) {
  return (relations.find(c) != relations.end());
}

bool ChunkRelations::IsCharacterClosing(const char& c) {
  for (auto r:relations) {
    if (r.second.first == c) {
      return true;
    }
  }

  return false;
}

int ChunkRelations::Score(const char& c) {
  for (auto r:relations) {
    if (r.second.first == c) {
      return r.second.second;
    }
  }

  throw invalid_argument("Not a closing character");
}

bool ChunkRelations::DoesCharacterBCloseA(const char& a, const char& b) {
  auto r = relations.find(a);
  if (r == relations.end()) {
    throw invalid_argument("Not an opening character");
  }
  return (r->second.first == b);
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day10/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // file input can be found as-is in: file_results.first
  // Initialize relations of characters
  ChunkRelations r;

  // Part 1:
  // Sum the syntax error scores of all lines
  unsigned long long int sum_corrupted_score = 0;

  for (auto line:file_results.first) {
    stack<int> s;
    for (int i = 0; i < line.size(); ++i) {
      // If a character is opening, then it is the start of a new chunk: stack it
      if (r.IsCharacterOpening(line[i])) {
        s.push(line[i]);
        continue;
      }

      // ... else it must be closing
      // If it is valid, it closes the last-seen opening character
      if (r.DoesCharacterBCloseA(s.top(), line[i])) {
        s.pop();
        continue;
      }
      // If it is invalid, but still a closing character, than this is a corrupted line
      // Stop at the first incorrect closing character on each corrupted line to determine the syntax error score for the line
      sum_corrupted_score += r.Score(line[i]);
      cout << "found a corrupted line" << endl;
      break; // go to the next line
    }

    cout << "size of stack after processing line: " << s.size() << endl;
  }

  cout << "Part 1 answer: " << sum_corrupted_score << endl;

  // Part 2:
  // TODO

  return 0;
}

// TODO ideas for future days: take in a flag from cmdline to run part 1 or part 2
