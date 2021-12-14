// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 13 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: How many dots are visible after completing just the first fold instruction on your transparent paper?
// Part 2: What code do you use to activate the infrared thermal imaging camera system? (8 capital letters)

#include "../util/fileutil.hpp" // ReadLinesFromFile, ParseSeparatedInts
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>

using namespace std;

// FoldPaper returns the dot map after the given paper state is folded over itself according to the given instruction
// x folds to the left; y folds up
// Note: the input looks like it folds in half every time, but if the fold instruction is not in the middle of initial_paper, will treat the overhang as part of the next paper stage
vector<vector<bool>> FoldPaper(const vector<vector<bool>>& initial_paper, const pair<string,int>& fold_instruction) {
  // Initialize the folded paper
  vector<vector<bool>> folded_paper;
  if (fold_instruction.first == "x") {
    folded_paper.resize(fold_instruction.second > initial_paper.size() - fold_instruction.second - 1 ? fold_instruction.second : initial_paper.size() - fold_instruction.second - 1);
  }
  else {
    folded_paper.resize(initial_paper.size());
  }
  for (int x = 0; x < folded_paper.size(); ++x) {
    if (fold_instruction.first == "y") {
      folded_paper[x].resize(fold_instruction.second > initial_paper[x].size() - fold_instruction.second - 1 ? fold_instruction.second : initial_paper[x].size() - fold_instruction.second - 1);
    }
    else {
      folded_paper[x].resize(initial_paper[x].size());
    }

    for (int y = 0; y < folded_paper[x].size(); ++y) {
      folded_paper[x][y] = false;
    }
  }

  // Fill in the folded paper by combining the initial values
  if (fold_instruction.first == "x") {
    for (int i = 1; i <= folded_paper.size(); ++i) {
      for (int y = 0; y < folded_paper[0].size(); ++y) {
        // check if a dot was present to the left of the fold
        if ((fold_instruction.second - i) >= 0) {
          if (initial_paper[fold_instruction.second - i][y]) {
            folded_paper[fold_instruction.second - i][y] = true;
            continue;
          }
        }
        else {
          cout << "Unexpected unequal fold instruction" << endl;
          return folded_paper;
        }
        // check if a dot was present to the right of the fold
        if ((fold_instruction.second + i) < initial_paper.size()) {
          if (initial_paper[fold_instruction.second + i][y]) {
            folded_paper[fold_instruction.second - i][y] = true;
            continue;
          }
        }
        else {
          cout << "Unexpected unequal fold instruction" << endl;
          return folded_paper;
        }
      }
    }
  }
  else if (fold_instruction.first == "y") {
    for (int x = 0; x < folded_paper.size(); ++x) {
      for (int i = 1; i <= folded_paper[x].size(); ++i) {
        // check if a dot was present above the fold
        if ((fold_instruction.second - i) >= 0) {
          if (initial_paper[x][fold_instruction.second - i]) {
            folded_paper[x][fold_instruction.second - i] = true;
            continue;
          }
        }
        else {
          cout << "Unexpected unequal fold instruction" << endl;
          return folded_paper;
        }
        // check if a dot was present below the fold
        if ((fold_instruction.second + i) < initial_paper[x].size()) {
          if (initial_paper[x][fold_instruction.second + i]) {
            folded_paper[x][fold_instruction.second - i] = true;
            continue;
          }
        }
        else {
          cout << "Unexpected unequal fold instruction" << endl;
          return folded_paper;
        }
      }
    }
  }
  else {
    cout << "Unexpected fold instruction dimension: " << fold_instruction.first << endl;
    return folded_paper;
  }

  return folded_paper;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day13/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  vector<pair<int,int>> dot_coordinates; // in pairs (x,y)
  int max_x = -1, max_y = -1;
  vector<pair<string,int>> fold_instructions;
  regex fold_rgx("fold along ([xy])=(\\d+)");
  bool fold_instructions_begin = false;
  
  // Parse file input
  for (auto line:file_results.first) {
    if (line == "") {
      fold_instructions_begin = true;
      continue;
    }

    // parse dot coordinates using the comma-separated regex helper function
    if (!fold_instructions_begin) {
      pair<vector<int>,int> coordinate_result = ParseSeparatedInts(line, ",");
      if (coordinate_result.second < 0 || coordinate_result.first.size() != 2) {
        cout << "Unexpected format of dot coordinate: " << line << endl;
        return -1;
      }

      int x = coordinate_result.first[0], y = coordinate_result.first[1];
      if (x > max_x) {
        max_x = x;
      }
      if (y > max_y) {
        max_y = y;
      }

      dot_coordinates.push_back(pair<int,int>(x, y));
    }
    // parse fold instructions
    else {
      smatch m;
      regex_search(line, m, fold_rgx);
      if (m.size() != 3) {
        cout << "Unexpected format of fold instruction: " << line << endl;
        return -1;
      }

      fold_instructions.push_back(pair<string,int>(m[1], stoi(m[2])));
    }
  }

  // Initialize starting state of paper
  vector<vector<bool>> paper; // accessed using [x][y] ; increasing x represents values moving to the right on the paper; increasing y represents values moving down the paper
  paper.resize(max_x+1);
  for (int x = 0; x <= max_x; ++x) {
    paper[x].resize(max_y+1);
    for (int y = 0; y <= max_y; ++y) {
      paper[x][y] = false;
    }
  }
  for (auto c:dot_coordinates) {
    paper[c.first][c.second] = true;
  }

  // Part 1:
  vector<vector<bool>> new_paper = FoldPaper(paper, fold_instructions[0]);

  int dot_count = 0;
  for (int x = 0; x < new_paper.size(); ++x) {
    for (int y = 0; y < new_paper[x].size(); ++y) {
      if (new_paper[x][y]) { ++dot_count; }
    }
  }
  cout << "Part 1 answer: " << dot_count << endl;

  // Part 2:
  for (int f = 1; f < fold_instructions.size(); ++f) {
    new_paper = FoldPaper(new_paper, fold_instructions[f]);
  }

  cout << "Part 2 answer:" << endl;
  // Print the final paper so it's easier to read
  // Turns out Letters are easiest to read iterating over y then x
  for (int y = 0; y < new_paper[0].size(); ++y) {
    for (int x = 0; x < new_paper.size(); ++x) {
      if (new_paper[x][y]) {
        cout << "#";
      }
      else {
        cout << " ";
      }
    }
    cout << endl;
  }

  return 0;
}
