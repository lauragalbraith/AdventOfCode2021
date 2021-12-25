// fileutil.cpp: Laura Galbraith
// Description: useful file-related functions for The Advent Of Code 2021

#include <tuple>
#include <vector>
#include <string>
#include <fstream>
#include <regex>

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

pair<vector<int>, int> ParseSeparatedInts(const string str, const string sep) {
  pair<vector<int>, int> results(vector<int>(), -1);
  string remaining_str = str;
  smatch m;
  string regex_str = "([-]{0,1}\\d+)" + sep;
  regex r(regex_str);
  while (regex_search(remaining_str, m, r)) {
    if (m.size() != 2) {
      return results;
    }

    int i = stoi(m[1]);
    results.first.push_back(i);
    remaining_str = m.suffix().str();
  }

  // cover the case where there is a listed int after the last separator
  if (remaining_str.length() > 0) {
    int last = stoi(remaining_str, NULL, 10);
    results.first.push_back(last);
  }

  results.second = 0;
  return results;
}
