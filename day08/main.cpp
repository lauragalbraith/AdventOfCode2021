// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 8 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: In the output values, how many times do digits 1, 4, 7, or 8 appear?
// Part 2: What do you get if you add up all of the output values?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <stdexcept>
#include <regex>

using namespace std;

const int kNumSignalPatterns = 10;
const int kNumOutputDigits = 4;

class SevenSegmentSignalPatterns {
  private:
    class SevenSegmentSignal {
      public:
        SevenSegmentSignal(const string signal_pattern) {
          this->a = false, this->b = false, this->c = false, this->d = false, this->e = false, this->f = false, this->g = false;
          for (string::const_iterator signal = signal_pattern.cbegin(); signal != signal_pattern.cend(); ++signal) {
            if (*signal == 'a') { this->a = true; }
            else if (*signal == 'b') { this->b = true; }
            else if (*signal == 'c') { this->c = true;  }
            else if (*signal == 'd') { this->d = true; }
            else if (*signal == 'e') { this->e = true; }
            else if (*signal == 'f') { this->f = true; }
            else if (*signal == 'g') { this->g = true; }
            else {
              throw invalid_argument("invalid seven-segment signal pattern: " + signal_pattern);
            }
          }
          this->represented_digit = -1;
        };
        bool a,b,c,d,e,f,g;
        int represented_digit;
        bool operator==(const SevenSegmentSignal& other) {
          return (this->a == other.a && this->b == other.b && this->c == other.c && this->d == other.d && this->e == other.e && this->f == other.f && this->g == other.g);
        };
    };

    vector<SevenSegmentSignal> unique_signals;

  public:
    SevenSegmentSignalPatterns(const vector<string> unique_signal_patterns);
    int RepresentedDigit(const string signal_pattern);
};

SevenSegmentSignalPatterns::SevenSegmentSignalPatterns(const vector<string> unique_signal_patterns) {
  if (unique_signal_patterns.size() != kNumSignalPatterns) {
    throw invalid_argument("invalid number of signal patterns");
  }

  for (int i = 0; i < kNumSignalPatterns; ++i) {
    this->unique_signals.push_back(SevenSegmentSignal(unique_signal_patterns[i]));
  }
}

// Based on the signal patterns stored, returns the digit that the given signal pattern represents
// Returns -1 if unknown or invalid
int SevenSegmentSignalPatterns::RepresentedDigit(const string signal_pattern) {
  // Compute answer if it's easy
  // digits 1, 4, 7, and 8 each use a unique number of segments -> (two, four, three, and seven respectively)
  int answer = -1;
  if (signal_pattern.length() == 2) {
    answer = 1;
  }
  else if (signal_pattern.length() == 4) {
    answer = 4;
  }
  else if (signal_pattern.length() == 3) {
    answer = 7;
  }
  else if (signal_pattern.length() == 7) {
    answer = 8;
  }

  // Check if answer is cached (and store it if we have it)
  SevenSegmentSignal signal(signal_pattern);
  for (auto s:this->unique_signals) {
    if (signal == s) { // == operator is overloaded for the type
      if (s.represented_digit >= 0) {
        if (s.represented_digit != answer) {
          throw logic_error("Cached digit value does not equal computed value");
        }

        return answer;
      }
      else if (answer >= 0) {
        s.represented_digit = answer;
        return answer;
      }

      // TODO in part 2 this may be the place where the answer is computed
    }
  }

  return -1;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day08/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  vector<pair<SevenSegmentSignalPatterns, vector<string>>> input;
  regex r("^([a-g]+) ([a-g]+) ([a-g]+) ([a-g]+) ([a-g]+) ([a-g]+) ([a-g]+) ([a-g]+) ([a-g]+) ([a-g]+) \\| ([a-g]+) ([a-g]+) ([a-g]+) ([a-g]+)$");

  for (auto line:file_results.first) {
    // parse line from file
    smatch m;
    regex_search(line, m, r);
    if (m.size() != 1 + kNumSignalPatterns + kNumOutputDigits) {
      cout << "Unexpected file format" << endl;
      return -1;
    }

    // form patterns object
    vector<string> unique_signal_patterns;
    for (int i = 1; i <= kNumSignalPatterns; ++i) {
      unique_signal_patterns.push_back(m[i]);
    }
    SevenSegmentSignalPatterns patterns(unique_signal_patterns);

    // form digits list
    vector<string> digits;
    for (int i = kNumSignalPatterns + 1; i < 1 + kNumSignalPatterns + kNumOutputDigits; ++i) {
      digits.push_back(m[i]);
    }

    input.push_back(pair<SevenSegmentSignalPatterns, vector<string>>(patterns, digits));
  }

  // Part 1:
  int count_easy_digits = 0;
  for (auto i = input.begin(); i != input.end(); ++i) {
    for (auto digit:i->second) {
      int decoded = i->first.RepresentedDigit(digit);
      if (decoded == 1 || decoded == 4 || decoded == 7 || decoded == 8) {
        ++count_easy_digits;
      }
    }
  }

  cout << "Part 1 answer: " << count_easy_digits << endl;

  // Part 2:
  // Use logic to figure out the value of each output digit
  // Compute the integer representation of the four-digit output
  // Sum up all the four-digit output numbers
  int sum = 0;

  cout << "Part 2 answer: " << sum << endl;

  return 0;
}
