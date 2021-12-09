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
#include <cmath>

using namespace std;

const int kNumSignalPatterns = 10;
const int kNumOutputDigits = 4;

// GetUniqueCharacter returns the character in a that is not in b
// The second value in the pair is negative if there was no character in a that is not in b
pair<char,int> GetUniqueCharacter(string a, string b) {
  if (a.size() != b.size() + 1) {
    return pair<char,int>('0', -1);
  }

  char unique = '0';
  int success = -1;
  for (auto ai = a.begin(); ai != a.end(); ++ai) {
    if (b.find(*ai) == string::npos) {
      if (unique == '0') {
        unique = *ai;
        success = 0;
      }
      else {
        // more than 1 unique character is not a case we're looking for
        return pair<char,int>('0', -1);
      }
    }
  }

  return pair<char,int>(unique, success);
}

class SevenSegmentSignal {
  public:
    SevenSegmentSignal(const string signal_pattern) : original_pattern(signal_pattern) {
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
    const string original_pattern;
    int represented_digit;
    bool operator==(const SevenSegmentSignal& other) {
      return (this->a == other.a && this->b == other.b && this->c == other.c && this->d == other.d && this->e == other.e && this->f == other.f && this->g == other.g);
    };
};

class SevenSegmentSignalPatterns {
  private:
    vector<SevenSegmentSignal> unique_signals;
    void DetermineRepresentedDigits(); // private so that it is only called when necessary

  public:
    SevenSegmentSignalPatterns(const vector<string> unique_signal_patterns);
    int RepresentedDigit(SevenSegmentSignal *signal_pattern);
};

SevenSegmentSignalPatterns::SevenSegmentSignalPatterns(const vector<string> unique_signal_patterns) {
  if (unique_signal_patterns.size() != kNumSignalPatterns) {
    throw invalid_argument("invalid number of signal patterns");
  }

  for (int i = 0; i < kNumSignalPatterns; ++i) {
    this->unique_signals.push_back(SevenSegmentSignal(unique_signal_patterns[i]));
  }
}

void SevenSegmentSignalPatterns::DetermineRepresentedDigits() {
  char A = '0', B = '0',C = '0',D = '0',E = '0',F = '0',G = '0'; // which segment they represent can be seen below in the diagram
    /*
 AAAA 
B    C
B    C
 DDDD 
E    F
E    F
 GGGG 
    */

  // Compute patterns from our starting knowledge:
  // digits 1, 4, 7, and 8 each use a unique number of segments -> (two, four, three, and seven respectively)
  string pattern1, pattern4, pattern7, pattern8;
  for (int i = 0; i < this->unique_signals.size(); ++i) {
    int known_rep = -1;
    if (this->unique_signals[i].original_pattern.length() == 2) {
      known_rep = 1;
      pattern1 = this->unique_signals[i].original_pattern;
    }
    else if (this->unique_signals[i].original_pattern.length() == 4) {
      known_rep = 4;
      pattern4 = this->unique_signals[i].original_pattern;
    }
    else if (this->unique_signals[i].original_pattern.length() == 3) {
      known_rep = 7;
      pattern7 = this->unique_signals[i].original_pattern;
    }
    else if (this->unique_signals[i].original_pattern.length() == 7) {
      known_rep = 8;
      pattern8 = this->unique_signals[i].original_pattern;
    }

    if (known_rep >= 0) {
      this->unique_signals[i].represented_digit = known_rep; // cache each int as we go
    }
  }

  // the A segment is the only segment different between the 1 and 7 values, which we have found
  auto a_segment = GetUniqueCharacter(pattern7, pattern1);
  if (a_segment.second < 0) {
    throw logic_error("A segment could not be determined as expected");
  }

  A = a_segment.first;

  // for the segments in the 1 value, C is present in eight signal patterns, where F is present in nine
  int count0 = 0, count1 = 0;
  for (auto p:this->unique_signals) {
    if (p.original_pattern.find(pattern1[0]) != string::npos) {
      ++count0;
    }
    if (p.original_pattern.find(pattern1[1]) != string::npos) {
      ++count1;
    }
  }

  if (count0 == 8 && count1 == 9) {
    C = pattern1[0];
    F = pattern1[1];
  }
  else if (count0 == 9 && count1 == 8) {
    F = pattern1[0];
    C = pattern1[1];
  }
  else {
    throw logic_error("Assumptions were incorrect about segments in 1-value");
  }

  // for the segments in the 4 value which are not in the 1 value, B is present in six patterns, where D is present in seven
  char opt0 = '0', opt1 = '0';
  for (auto c:pattern4) {
    if (pattern1.find(c) == string::npos) {
      if (opt0 == '0') {
        opt0 = c;
      }
      else {
        opt1 = c;
        break;
      }
    }
  }

  if (opt0 == '0' || opt1 == '0') {
    throw logic_error("Segments in 4 value were not separated as expected");
  }

  count0 = 0, count1 = 0;
  for (auto p:this->unique_signals) {
    if (p.original_pattern.find(opt0) != string::npos) {
      ++count0;
    }
    if (p.original_pattern.find(opt1) != string::npos) {
      ++count1;
    }
  }

  if (count0 == 6 && count1 == 7) {
    B = opt0;
    D = opt1;
  }
  else if (count0 == 7 && count1 == 6) {
    D = opt0;
    B = opt1;
  }
  else {
    throw logic_error("Assumptions were incorrect about segments in 4-value");
  }

  // determine G segment as it is in the 9 value with one additional value on top of A, C, F, B, D
  string acfbd({A, C, F, B, D});
  string pattern9 = "";
  for (int i = 0; i < this->unique_signals.size(); ++i) {
    auto result = GetUniqueCharacter(this->unique_signals[i].original_pattern, acfbd);
    if (result.second >= 0) {
      G = result.first;
      pattern9 = this->unique_signals[i].original_pattern;
      this->unique_signals[i].represented_digit = 9; // cache the int since we know it here
      break;
    }
  }

  if (pattern9.size() <= 0) {
    throw logic_error("G segment could not be determined as expected");
  }

  // determine E segment as it is in the 8 value but not the 9 value
  auto result = GetUniqueCharacter(pattern8, pattern9);
  E = result.first;

  // Cache remaining int representations: 0, 2, 3, 5, 6 (we already know 1, 4, 7, 8, 9)
  SevenSegmentSignal signal0(string({A, C, F, G, E, B}));
  SevenSegmentSignal signal2(string({A, C, D, E, G}));
  SevenSegmentSignal signal3(string({A, C, D, F, G}));
  SevenSegmentSignal signal5(string({A, B, D, F, G}));
  SevenSegmentSignal signal6(string({A, B, D, F, E, G}));

  for (int i = 0; i < this->unique_signals.size(); ++i) {
    if (this->unique_signals[i] == signal0) {
      this->unique_signals[i].represented_digit = 0;
    }
    else if (this->unique_signals[i] == signal2) {
      this->unique_signals[i].represented_digit = 2;
    }
    else if (this->unique_signals[i] == signal3) {
      this->unique_signals[i].represented_digit = 3;
    }
    else if (this->unique_signals[i] == signal5) {
      this->unique_signals[i].represented_digit = 5;
    }
    else if (this->unique_signals[i] == signal6) {
      this->unique_signals[i].represented_digit = 6;
    }
    else if (this->unique_signals[i].represented_digit < 0) {
      throw logic_error("represented digit should be known for " + this->unique_signals[i].original_pattern);
    }
  }
}

// Based on the signal patterns stored, returns the digit that the given signal pattern represents
// Returns -1 if unknown or invalid
int SevenSegmentSignalPatterns::RepresentedDigit(SevenSegmentSignal *signal_pattern) {
  int answer = -1;
  // Check if answer is cached (and store it if we have it)
  SevenSegmentSignal* matching;
  for (int i = 0; i < this->unique_signals.size(); ++i) {
    if (*signal_pattern == this->unique_signals[i]) { // == operator is overloaded for the type
      matching = &(this->unique_signals[i]);
      if (this->unique_signals[i].represented_digit >= 0) {
        answer = this->unique_signals[i].represented_digit;
        break;
      }
    }
  }


  // If answer still isn't known, solve the whole puzzle and then determine
  if (answer < 0) {
    this->DetermineRepresentedDigits();
    answer = matching->represented_digit;
  }

  signal_pattern->represented_digit = answer; // set digit in the parameter as well
  return answer;
}

class OutputNumber {
  private:
    vector<SevenSegmentSignal> digits; // ordered from leftmost digit to rightmost
  public:
    OutputNumber(const vector<string> strs);
    vector<SevenSegmentSignal*> GetDigits();
    double RepresentedNumber();
};

OutputNumber::OutputNumber(const vector<string> strs) {
  if (strs.size() != kNumOutputDigits) {
    throw invalid_argument("invalid length of output number strings");
  }

  for (auto str:strs) {
    SevenSegmentSignal signal(str);
    this->digits.push_back(signal);
  }
}

vector<SevenSegmentSignal*> OutputNumber::GetDigits() {
  vector<SevenSegmentSignal*> ret;
  for (int i = 0; i < this->digits.size(); ++i) {
    ret.push_back(&(this->digits[i]));
  }
  return ret;
}

double OutputNumber::RepresentedNumber() {
  double value = 0;

  for (int i = 0; i < this->digits.size(); ++i) {
    int digit_value = this->digits[i].represented_digit;
    if (digit_value < 0 || digit_value > 9) {
      throw logic_error("invalid digit value");
    }

    value += digit_value * pow(10, this->digits.size() - i - 1);
  }

  return value;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day08/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  vector<pair<SevenSegmentSignalPatterns, OutputNumber>> input;
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
    OutputNumber o(digits);

    input.push_back(pair<SevenSegmentSignalPatterns, OutputNumber>(patterns, o));
  }

  // Part 1:
  int count_easy_digits = 0;
  for (int i = 0; i < input.size(); ++i) {
    for (auto digit:input[i].second.GetDigits()) {
      int decoded = input[i].first.RepresentedDigit(digit);
      if (decoded == 1 || decoded == 4 || decoded == 7 || decoded == 8) {
        ++count_easy_digits;
      }
    }
  }

  cout << "Part 1 answer: " << count_easy_digits << endl;

  // Part 2:
  unsigned long long int sum = 0;
  // Use logic to figure out the value of each output digit
  for (int i = 0; i < input.size(); ++i) {
    for (auto digit:input[i].second.GetDigits()) {
      input[i].first.RepresentedDigit(digit); // solve for all 4 of the represented digits
    }

    // Compute the integer representation of the four-digit output
    sum += input[i].second.RepresentedNumber();
  }
  
  // Sum up all the four-digit output numbers
  cout << "Part 2 answer: " << sum << endl;

  return 0;
}
