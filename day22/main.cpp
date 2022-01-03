// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 22 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: Considering only cubes in the region x=-50..50,y=-50..50,z=-50..50, how many cubes are on?
// Part 2: Considering all cubes, how many cubes are on?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>
#include <cmath> // TODO FINALLY remove if unneeded

using namespace std;

class Step {
  public: static const int INIT_PROC_MIN_LIMIT = -50;
  public: static const int INIT_PROC_MAX_LIMIT = 50;

  private:
    const bool state;
    const int x_min, x_max, y_min, y_max, z_min, z_max;

    static const bool NumWithinInitProcLimits(const int& a) { return a >= Step::INIT_PROC_MIN_LIMIT && a <= Step::INIT_PROC_MAX_LIMIT; }

  public:
    Step(const bool& s,
      const int& minx,const int& maxx,
      const int& miny, const int& maxy,
      const int& minz, const int& maxz):
      state(s),
      x_min(minx), x_max(maxx),
      y_min(miny), y_max(maxy),
      z_min(minz), z_max(maxz) {}

    const bool GetState() const { return this->state; }
    const pair<int,int> GetXRange() const { return pair<int,int>(this->x_min, this->x_max); }
    const pair<int,int> GetYRange() const { return pair<int,int>(this->y_min, this->y_max); }
    const pair<int,int> GetZRange() const { return pair<int,int>(this->z_min, this->z_max); }
    const bool WithinInitializationProcedure() const;

    ~Step() {}
};

const bool Step::WithinInitializationProcedure() const {
  return Step::NumWithinInitProcLimits(this->x_min) &&
    Step::NumWithinInitProcLimits(this->x_max) &&
    Step::NumWithinInitProcLimits(this->y_min) &&
    Step::NumWithinInitProcLimits(this->y_max) &&
    Step::NumWithinInitProcLimits(this->z_min) &&
    Step::NumWithinInitProcLimits(this->z_max);
}

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const Step& s) {
  os << (s.GetState() ? "on" : "off") << " x=" << s.GetXRange().first << ".." << s.GetXRange().second << ",y=" << s.GetYRange().first << ".." << s.GetYRange().second << ",z=" << s.GetZRange().first << ".." << s.GetZRange().second;
  return os;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day22/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  int min_x = 0, max_x = 0, min_y = 0, max_y = 0, min_z = 0, max_z = 0;
  vector<Step> steps;

  regex r("([onf]+) x=([-]{0,1}\\d+)..([-]{0,1}\\d+),y=([-]{0,1}\\d+)..([-]{0,1}\\d+),z=([-]{0,1}\\d+)..([-]{0,1}\\d+)");
  for (auto line:file_results.first) {
    smatch m;
    regex_search(line, m, r);
    if (m.size() != 8) {
      cout << "Unexpected number of matches from regex on input line: " << line << ": " << m.size() << endl;
      return -1;
    }

    int step_min_x = stoi(m[2]), step_max_x = stoi(m[3]);
    if (step_min_x > step_max_x) {
      cout << "Expected " << step_min_x << " to be larger than " << step_max_x << " in x dimensions of line " << line << endl;
      return -1;
    }
    
    if (step_min_x < min_x) {
      min_x = step_min_x;
    }
    if (step_max_x > max_x) {
      max_x = step_max_x;
    }

    int step_min_y = stoi(m[4]), step_max_y = stoi(m[5]);
    if (step_min_y > step_max_y) {
      cout << "Expected " << step_min_y << " to be larger than " << step_max_y << " in y dimensions of line " << line << endl;
      return -1;
    }

    if (step_min_y < min_y) {
      min_y = step_min_y;
    }
    if (step_max_y > max_y) {
      max_y = step_max_y;
    }

    int step_min_z = stoi(m[6]), step_max_z = stoi(m[7]);
    if (step_min_z > step_max_z) {
      cout << "Expected " << step_min_z << " to be larger than " << step_max_z << " in z dimensions of line " << line << endl;
      return -1;
    }

    if (step_min_z < min_z) {
      min_z = step_min_z;
    }
    if (step_max_z > max_z) {
      max_z = step_max_z;
    }

    if (m[1] == "on") {
      steps.push_back(Step(true, step_min_x, step_max_x, step_min_y, step_max_y, step_min_z, step_max_z));
    }
    else if (m[1] == "off") {
      steps.push_back(Step(false, step_min_x, step_max_x, step_min_y, step_max_y, step_min_z, step_max_z));
    }
    else {
      cout << "Invalid directive from step: " << m[1] << endl;
      return -1;
    }
  }

  // TODO FINALLY remove debugging statements
  // cout << "X: " << min_x << ":" << max_x << endl;
  // cout << "Y: " << min_y << ":" << max_y << endl;
  // cout << "Z: " << min_z << ":" << max_z << endl;

  // Part 1: since the problem is bounded, take the easiest implementation to track "on" cubes
  vector<vector<vector<bool>>> init_grid; // accessed like [x-INIT_PROC_MIN_LIMIT][y-INIT_PROC_MIN_LIMIT][z-INIT_PROC_MIN_LIMIT]
  const int bound_size = Step::INIT_PROC_MAX_LIMIT - Step::INIT_PROC_MIN_LIMIT + 1;
  init_grid.resize(bound_size);
  for (int x = 0; x < bound_size; ++x) {
    init_grid[x].resize(bound_size);
    for (int y = 0; y < bound_size; ++y) {
      init_grid[x][y].resize(bound_size);
      for (int z = 0; z < bound_size; ++z) {
        init_grid[x][y][z] = false; // initialize to "off"
      }
    }
  }

  for (auto s:steps) {
    if (s.WithinInitializationProcedure()) {
      auto x_range = s.GetXRange();
      auto y_range = s.GetYRange();
      auto z_range = s.GetZRange();
      for (int x = x_range.first; x <= x_range.second; ++x) {
        for (int y = y_range.first; y <= y_range.second; ++y) {
          for (int z = z_range.first; z <= z_range.second; ++z) {
            init_grid[x - Step::INIT_PROC_MIN_LIMIT][y - Step::INIT_PROC_MIN_LIMIT][z - Step::INIT_PROC_MIN_LIMIT] = s.GetState();
          }
        }
      }
    }
  }

  int on_count = 0;
  for (auto x:init_grid) {
    for (auto y:x) {
      for (auto z:y) {
        if (z) { ++on_count; }
      }
    }
  }

  cout << "Part 1 answer: " << on_count << endl;

  // Part 2:
  // TODO ideas: we can't store all the coordinates in memory (initializing them takes long enough); we could store only "on" coordinates (map of int x to [map of int y to [map of [int z to state]]]? map of coordinate types to on? store on "intervals", i.e. cuboids, where if a cuboid is interrupted by a following step, it divides into up to 26 smaller cuboids sectioning off the on-world around the cuboid that has been turned off? I think this uses the least space, but will be the most complex to code)

  return 0;
}
