// main.cpp: Laura Galbraith
// Description: solver for Puzzle 1 and 2 of Day 2 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// What do you get if you multiply your final horizontal position by your final depth?
// Part 1 and Part 2 treat the directions differently

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <regex>
#include <stdexcept>

using namespace std;

// O(n) where n is the number of lines in the file
const vector<string> GetInput(const string file_name) {
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

// O(m) where m is the length of the longer of regex/string
int ExtractIntFromStringUsingRegex(const regex r, const string s) {
  smatch m;
  if (!regex_search(s, m, r)) {
    cout << "No match found in search of string: " << s << endl;
    return -1;
  }
  if (m.size() != 2) {
    cout << "Unexpected number of matches in string: " << s << endl;
    return -1;
  }

  string match = m.str(1);
  int ret;
  try {
    ret = stoi(match, NULL);
  }
  catch (invalid_argument& ia) {
    cout << "Invalid argument exception was caught parsing an int from the matched string: " << ia.what() << endl;
    ret = -1;
  }
  return ret;
}

// O(1) since regexes and strings are short
// Pairs are ordered <horizontal position, depth> - TODO clarify contract by creating a type
pair<int, int> CalculateNewHDPosition(pair<int, int> curr_pos, const string step) {
  int h = curr_pos.first;
  int d = curr_pos.second;

  pair<int, int> new_pos(h, d);
  pair<int, int> error(-1, -1);
  regex forward("^forward (\\d+)$"), down("^down (\\d+)$"), up("^up (\\d+)$");

  if (regex_match(step, forward)) {
    int h_diff = ExtractIntFromStringUsingRegex(forward, step);
    if (h_diff == -1) {
      return error;
    }
    new_pos.first = h + h_diff;
  }
  else if (regex_match(step, down)) {
    int d_diff = ExtractIntFromStringUsingRegex(down, step);
    if (d_diff == -1) {
      return error;
    }
    new_pos.second = d + d_diff;
  }
  else if (regex_match(step, up)) {
    int d_diff = ExtractIntFromStringUsingRegex(up, step);
    if (d_diff == -1) {
      return error;
    }
    new_pos.second = d - d_diff;
  }
  else {
    cout << "Step did not match any recognized directions" << endl;
    return error;
  }

  return new_pos;
}

// O(1) since regexes and strings are short
// Pairs are ordered <horizontal position, depth, aim>
tuple<int, int, int> CalculateNewHDAimPosition(tuple<int, int, int> curr_pos_aim, const string step) {
  int h = get<0>(curr_pos_aim);
  int d = get<1>(curr_pos_aim);
  int aim = get<2>(curr_pos_aim);

  tuple<int, int, int> error(-1, -1, -1);
  regex forward("^forward (\\d+)$"), down("^down (\\d+)$"), up("^up (\\d+)$"); // TODO move to global

  if (regex_match(step, forward)) {
    int h_diff = ExtractIntFromStringUsingRegex(forward, step);
    if (h_diff == -1) {
      return error;
    }

    // forward X does two things:
    //  It increases your horizontal position by X units.
    //  It increases your depth by your aim multiplied by X.
    tuple<int, int, int> new_pos_aim(h + h_diff, d + (aim*h_diff), aim);
    return new_pos_aim;
  }
  else if (regex_match(step, down)) {
    int d_diff = ExtractIntFromStringUsingRegex(down, step);
    if (d_diff == -1) {
      return error;
    }

    // down X increases your aim by X units
    tuple<int, int, int> new_pos_aim(h, d, aim + d_diff);
    return new_pos_aim;
  }
  else if (regex_match(step, up)) {
    int d_diff = ExtractIntFromStringUsingRegex(up, step);
    if (d_diff == -1) {
      return error;
    }

    // up X decreases your aim by X units
    tuple<int, int, int> new_pos_aim(h, d, aim - d_diff);
    return new_pos_aim;
  }
  else {
    cout << "Step did not match any recognized directions" << endl;
    return error;
  }
}

int main() {
  vector<string> file_contents = GetInput("input.txt");
  if (file_contents.size() <= 0) {
    cout << "No file contents found" << endl;
    return -1;
  }

  // Part 1
  pair<int, int> curr_pos(0,0);
  for (vector<string>::const_iterator i = file_contents.begin(); i != file_contents.end(); ++i) {
    curr_pos = CalculateNewHDPosition(curr_pos, *i);
  }

  cout << "Part 1 Final position: " << curr_pos.first << " " << curr_pos.second << endl;
  cout << "Part 1 Answer: " << curr_pos.first * curr_pos.second << endl;

  // Part 2
  tuple<int, int, int> curr_pos_aim(0,0,0);
  for (vector<string>::const_iterator i = file_contents.begin(); i != file_contents.end(); ++i) {
    curr_pos_aim = CalculateNewHDAimPosition(curr_pos_aim, *i);
  }

  cout << "Part 2 Final position: " << get<0>(curr_pos_aim) << " " << get<1>(curr_pos_aim) << endl;
  cout << "Part 2 Answer: " << get<0>(curr_pos_aim) * get<1>(curr_pos_aim) << endl;
  
  return 0;
}
