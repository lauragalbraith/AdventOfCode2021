// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 12 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: How many paths through this cave system are there that visit small caves at most once?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <sstream>

using namespace std;

// Constants used commonly in this puzzle
const string start_cave_name = "start";
const string end_cave_name = "end";

// Cave class represents a cave in this puzzle; a node in a copy of the graph
class Cave {
  private:
    vector<Cave*> adjacent_caves;
    const string name;
  public:
    Cave(const string& n);
    void AddAdjacentCave(Cave* other);
    vector<Cave*> GetAdjacentCaves();
    const string& GetName();
    bool IsSmall();

    bool discovered;
};

Cave::Cave(const string& n) : name(n) {
  this->adjacent_caves.resize(0);
  this->discovered = false;
}

void Cave::AddAdjacentCave(Cave* other) {
  // assumes that this will never be called twice on the same values of 'this' and 'other'
  this->adjacent_caves.push_back(other);
}

vector<Cave*> Cave::GetAdjacentCaves() {
  return this->adjacent_caves;
}

const string& Cave::GetName() {
  return this->name;
}

bool Cave::IsSmall() {
  // equivalent to see if the first character is lowercase, because the casing is consistent
  return (this->name[0] <= 'z' && this->name[0] >= 'a');
}

// DepthFirstPaths follows the recursive DFS algorithm to return a list of all paths from c to end, using the provided path so far
const vector<string> DepthFirstPaths(Cave* c, const string& path_so_far) {
  // Mark the cave as discovered, unless it is a big cave (which can be visited more than once in a single path) or end cave, which is specifically detected
  if ((c->IsSmall() || c->GetName() == start_cave_name) && c->GetName() != end_cave_name) {
    c->discovered = true;
  }

  // Include the cave on the path so far
  stringstream s;
  s << path_so_far;
  if (path_so_far != "") {
    s << ",";
  }
  s << c->GetName();
  string current_path = s.str();

  vector<string> all_paths;

  // Check if we've reached the end of the caves
  if (c->GetName() == end_cave_name) {
    all_paths.push_back(current_path);
    return all_paths;
  }

  // For all adjacent caves to c...
  for (auto a:c->GetAdjacentCaves()) {
    // ... if the adjacent cave is not discovered yet, recursively compute paths from it
    if (!a->discovered) {
      vector<string> paths = DepthFirstPaths(a, current_path);
      for (auto p:paths) {
        all_paths.push_back(p);
      }
    }
  }

  // Now that we've explored all paths through this cave, unmark it so other paths called outside this recursion can use it anew
  if (c->discovered) {
    c->discovered = false;
  }

  return all_paths;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day12/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  // for initialization/cleanup ease, keep a reference to all caves
  map<string,Cave*> all_caves;
  regex r("([a-zA-Z]+)\\-([a-zA-Z]+)");
  for (auto line:file_results.first) {
    // parse two connected caves from line
    smatch m;
    regex_search(line, m, r);
    if (m.size() != 3) {
      cout << "Invalid line format: " << line << endl;
      return -1;
    }

    Cave* cave1 = NULL;
    Cave* cave2 = NULL;
    // if the first cave hasn't been created yet, create it
    string cave1_name = m[1];
    auto cave1_it = all_caves.find(cave1_name);
    if (cave1_it == all_caves.end()) {
      cave1 = new Cave(cave1_name);
      all_caves[cave1_name] = cave1;
    }
    else {
      cave1 = cave1_it->second;
    }

    // if the second cave hasn't been created yet, create it
    string cave2_name = m[2];
    auto cave2_it = all_caves.find(cave2_name);
    if (cave2_it == all_caves.end()) {
      cave2 = new Cave(cave2_name);
      all_caves[cave2_name] = cave2;
    }
    else {
      cave2 = cave2_it->second;
    }

    // connect the first cave to the second cave
    cave1->AddAdjacentCave(cave2);
    // connect the second cave to the first cave
    cave2->AddAdjacentCave(cave1);
  }

  // Part 1:
  // Get 'start' cave
  Cave* start_cave = all_caves.find(start_cave_name)->second;
  
  // Perform a depth-first search to determine all paths from 'start' cave to 'end' cave
  // For Part 1, at least, big caves (uppercase letters) can be visited more than once on a path, but small caves cannot
  // This means that small caves (and start/end) will be marked "discovered" in the process of carving a path, but big caves will not
  vector<string> all_paths = DepthFirstPaths(start_cave, "");

  cout << "Part 1 answer: " << all_paths.size() << endl;

  // Part 2:
  // TODO

  // Free all memory allocated
  for (auto cave:all_caves) {
    delete cave.second;
  }

  return 0;
}
