// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 13 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: How many dots are visible after completing just the first fold instruction on your transparent paper?
// Part 2: TODO

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
  cout << "fold instruction is " << fold_instruction.first << " over " << fold_instruction.second << endl;
  cout << "New size of paper is ";
  if (fold_instruction.first == "x") {
    folded_paper.resize(fold_instruction.second > initial_paper.size() - fold_instruction.second - 1 ? fold_instruction.second : initial_paper.size() - fold_instruction.second - 1);
  }
  else {
    folded_paper.resize(initial_paper.size());
  }
  cout << "max x:" << folded_paper.size() - 1;
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
  cout << " max y:" << folded_paper[0].size() - 1 << endl;

  // Fill in the folded paper by combining the initial values
  if (fold_instruction.first == "x") {
    for (int i = 1; i <= folded_paper.size(); ++i) {
      for (int y = 0; y < folded_paper[0].size(); ++y) { // TODO HERE looking over code for issues, focusing on x directions
        // cout << "Checking if x=" << fold_instruction.second - i << " should be filled in at y=" << y;
        // check if a dot was present to the left of the fold
        if ((fold_instruction.second - i) >= 0) {
          if (initial_paper[fold_instruction.second - i][y]) {
            // cout << "; yes because x=" << fold_instruction.second - i << ", y=" << y << "was true in initial set";
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
            // cout << ": yes because x=" << fold_instruction.second + i << ", y=" << y << "was true in initial set";
            folded_paper[fold_instruction.second - i][y] = true;
            continue;
          }
        }
        else {
          cout << "Unexpected unequal fold instruction" << endl;
          return folded_paper;
        }
        // cout << endl;
      }
    }
  }
  // TODO probably Part 2: the else condition where it's y that's changing

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
  cout << "Initial paper is max x:" << max_x << " by max y:" << max_y << endl;
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
  cout << "Part 1 answer: " << dot_count << endl; // TODO HERE 490 was too low

  // Part 2:
  // TODO

  return 0;
}
