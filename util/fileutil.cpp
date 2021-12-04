// fileutil.cpp: Laura Galbraith
// Description: useful file-related functions for The Advent Of Code 2021

#include <tuple>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

pair<vector<string>, int> ReadLinesFromFile(const string file_name) {
  // Read in file
  ifstream f(file_name);
  vector<string> contents;
  if (f.is_open()) {
    string line;
    while (getline(f, line)) {
      contents.push_back(line);
    }
  }
  else {
    pair<vector<string>, int> error(NULL, -1);
    return error;
  }

  pair<vector<string>, int> result(contents, 0);
  return result;
}
