// main.cpp: Laura Galbraith
// Description: solver for Puzzle 1 and 2 of Day 2 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// What do you get if you multiply your final horizontal position by your final depth?
// Part 1 and Part 2 treat the directions differently

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <regex>
#include <stdexcept>
#include <tuple>

using namespace std;

class PositionAim {
  private:
    int horizontal_position;
    int depth_position;
    int aim;
  public:
    bool success;
    PositionAim(int h, int d, int a);
    PositionAim(bool s);
    int get_horizontal_position() {return horizontal_position;}
    int get_depth_position() {return depth_position;}
    int get_aim() {return aim;}
    int calculate_multiple_position() {return horizontal_position*depth_position;}
};

PositionAim::PositionAim(bool s) {
  horizontal_position = -1;
  depth_position = -1;
  aim = -1;
  success = s;
}

PositionAim::PositionAim(int h, int d, int a) {
  horizontal_position = h;
  depth_position = d;
  aim = a;
  success = true;
}

const regex forward_rgx("^forward (\\d+)$"), down_rgx("^down (\\d+)$"), up_rgx("^up (\\d+)$");

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
PositionAim CalculateNewHDPosition(PositionAim curr_pos, const string step) {
  int h = curr_pos.get_horizontal_position();
  int d = curr_pos.get_depth_position();

  PositionAim error(false);
  if (regex_match(step, forward_rgx)) {
    int h_diff = ExtractIntFromStringUsingRegex(forward_rgx, step);
    if (h_diff == -1) {
      return error;
    }

    PositionAim new_pos(h + h_diff, d, -1);
    return new_pos;
  }
  else if (regex_match(step, down_rgx)) {
    int d_diff = ExtractIntFromStringUsingRegex(down_rgx, step);
    if (d_diff == -1) {
      return error;
    }
    
    PositionAim new_pos(h, d + d_diff, -1);
    return new_pos;
  }
  else if (regex_match(step, up_rgx)) {
    int d_diff = ExtractIntFromStringUsingRegex(up_rgx, step);
    if (d_diff == -1) {
      return error;
    }
    
    PositionAim new_pos(h, d - d_diff, -1);
    return new_pos;
  }
  else {
    cout << "Step did not match any recognized directions" << endl;
    return error;
  }
}

// O(1) since regexes and strings are short
PositionAim CalculateNewHDAimPosition(PositionAim curr_pos_aim, const string step) {
  int h = curr_pos_aim.get_horizontal_position();
  int d = curr_pos_aim.get_depth_position();
  int aim = curr_pos_aim.get_aim();

  PositionAim error(false);
  if (regex_match(step, forward_rgx)) {
    int h_diff = ExtractIntFromStringUsingRegex(forward_rgx, step);
    if (h_diff == -1) {
      return error;
    }

    // forward X does two things:
    //  It increases your horizontal position by X units.
    //  It increases your depth by your aim multiplied by X.
    PositionAim new_pos_aim(h + h_diff, d + (aim*h_diff), aim);
    return new_pos_aim;
  }
  else if (regex_match(step, down_rgx)) {
    int d_diff = ExtractIntFromStringUsingRegex(down_rgx, step);
    if (d_diff == -1) {
      return error;
    }

    // down X increases your aim by X units
    PositionAim new_pos_aim(h, d, aim + d_diff);
    return new_pos_aim;
  }
  else if (regex_match(step, up_rgx)) {
    int d_diff = ExtractIntFromStringUsingRegex(up_rgx, step);
    if (d_diff == -1) {
      return error;
    }

    // up X decreases your aim by X units
    PositionAim new_pos_aim(h, d, aim - d_diff);
    return new_pos_aim;
  }
  else {
    cout << "Step did not match any recognized directions" << endl;
    return error;
  }
}

int main() {
  pair<vector<string>, int> file_result = ReadLinesFromFile("day2/input.txt");
  if (file_result.second < 0) {
    cout << "File could not be opened" << endl;
    return -1;
  }

  // Part 1
  PositionAim curr_pos(0, 0, -1);
  for (vector<string>::const_iterator i = file_result.first.begin(); i != file_result.first.end(); ++i) {
    curr_pos = CalculateNewHDPosition(curr_pos, *i);
    if (!curr_pos.success) {
      cout << "Calculating new position failed" << endl;
      return -1;
    }
  }

  cout << "Part 1 Final position: " << curr_pos.get_horizontal_position() << " " << curr_pos.get_depth_position() << endl;
  cout << "Part 1 Answer: " << curr_pos.calculate_multiple_position() << endl;

  // Part 2
  PositionAim curr_pos_aim(0, 0, 0);
  for (vector<string>::const_iterator i = file_result.first.begin(); i != file_result.first.end(); ++i) {
    curr_pos_aim = CalculateNewHDAimPosition(curr_pos_aim, *i);
    if (!curr_pos.success) {
      cout << "Calculating new position failed" << endl;
      return -1;
    }
  }

  cout << "Part 2 Final position: " << curr_pos_aim.get_horizontal_position() << " " << curr_pos_aim.get_depth_position() << endl;
  cout << "Part 2 Answer: " << curr_pos_aim .calculate_multiple_position()<< endl;
  
  return 0;
}
