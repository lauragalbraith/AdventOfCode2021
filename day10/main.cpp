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
#include <algorithm>

using namespace std;

class ChunkRelations {
  private:
    map<char,char> open_close;
    map<char,int> closing_syntax_error_scores;
    map<char,int> closing_autocomplete_scores;
  public:
    ChunkRelations();
    bool IsCharacterOpening(const char& c);
    bool IsCharacterClosing(const char& c);
    const char& ClosingCharacterForA(const char& a); // TODO FINALLY rename parameter and function to not mention A
    int SyntaxErrorScore(const char& c);
    unsigned long long int AutocompleteScore(const string& s);
};

ChunkRelations::ChunkRelations() {
  this->open_close['('] = ')';
  this->open_close['['] = ']';
  this->open_close['{'] = '}';
  this->open_close['<'] = '>';

  this->closing_syntax_error_scores[')'] = 3;
  this->closing_syntax_error_scores[']'] = 57;
  this->closing_syntax_error_scores['}'] = 1197;
  this->closing_syntax_error_scores['>'] = 25137;

  this->closing_autocomplete_scores[')'] = 1;
  this->closing_autocomplete_scores[']'] = 2;
  this->closing_autocomplete_scores['}'] = 3;
  this->closing_autocomplete_scores['>'] = 4;
}

bool ChunkRelations::IsCharacterOpening(const char& c) {
  return (open_close.find(c) != open_close.end()); // TODO FINALLY add this-> to all references in ChunkRelations functions
}

bool ChunkRelations::IsCharacterClosing(const char& c) {
  return (this->closing_syntax_error_scores.find(c) != this->closing_syntax_error_scores.end());
}

const char& ChunkRelations::ClosingCharacterForA(const char& a) {
  auto r = open_close.find(a);
  if (r == open_close.end()) {
    throw invalid_argument("Not an opening character");
  }
  return r->second;
}

int ChunkRelations::SyntaxErrorScore(const char& c) {
  auto r = closing_syntax_error_scores.find(c);
  if (r == closing_syntax_error_scores.end()) {
    throw invalid_argument("Not a closing character");
  }
  return r->second;
}

unsigned long long int ChunkRelations::AutocompleteScore(const string& s) {
  unsigned long long int score = 0;

  for (int i = 0; i < s.size(); ++i) {
    int char_score = this->closing_autocomplete_scores[s[i]];
    score = score * 5 + char_score;
  }

  return score;
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
  // Sum the syntax error scores of all corrupted lines
  unsigned long long int sum_corrupted_score = 0;
  vector<unsigned long long int> autocomplete_scores;

  for (auto line:file_results.first) {
    stack<int> s;
    bool corrupted = false;
    for (int i = 0; i < line.size(); ++i) {
      // If a character is opening, then it is the start of a new chunk: stack it
      if (r.IsCharacterOpening(line[i])) {
        s.push(line[i]);
        continue;
      }

      // ... else it must be closing
      // If it closes the last-seen opening character, it is valid
      cout << "want to close " << s.top() << " with " << r.ClosingCharacterForA(s.top()) << endl;
      if (r.ClosingCharacterForA(s.top()) == line[i]) {
        cout << "we have succeeded!" << endl;
        s.pop();
        continue;
      }
      
      // If it is invalid, but still a closing character, than this is a corrupted line
      corrupted = true;
      // Stop at the first incorrect closing character on each corrupted line to determine the syntax error score for the line
      sum_corrupted_score += r.SyntaxErrorScore(line[i]);
      cout << "found corrupted character " << line[i] << " and now score is " << sum_corrupted_score << endl;
      break; // stop processing characters in the line
    }

    // Part 2:
    // All non-corrupted lines are incomplete
    // Using the stack, determine the appropriate closing characters needed and determine its score
    cout << "size of the stack is " << s.size() << endl;
    if (!corrupted) {
      string missing_closing_chars = "";
      while (!s.empty()) {
        missing_closing_chars.append(1, r.ClosingCharacterForA(s.top()));
        s.pop();
      }
      autocomplete_scores.push_back(r.AutocompleteScore(missing_closing_chars));
    }
  }

  cout << "Part 1 answer: " << sum_corrupted_score << endl;

  sort(autocomplete_scores.begin(), autocomplete_scores.end());
  cout << "Part 2 answer: " << autocomplete_scores[autocomplete_scores.size() / 2] << endl;

  return 0;
}

// TODO ideas for future days: take in a flag from cmdline to run part 1 or part 2; add compiler warnings/errors (-Wall, -Werror, etc.)
