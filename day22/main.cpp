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
#include <stdexcept>
#include <queue>

using namespace std;

// IntWithinRange returns true if x is within the specified range limits (inclusive)
bool IntWithinRange(const int x, const int range_min, const int range_max) {
  return x >= range_min && x <= range_max;
}

// RangeOverlap returns the range of overlap between the two given ranges, or false as the 2nd pair element if there is no element
pair<pair<int,int>, bool> RangeOverlap(const pair<int,int>& a, const pair<int,int>& b) {
  // Order ranges starting with the range with the smallest min element
  pair<int,int> r1 = a.first < b.first ? a : b;
  pair<int,int> r2 = r1 == a ? b : a;

  // If r2 does not start within r1 (since we know r1 starts first), there is no overlap
  if (IntWithinRange(r2.first, r1.first, r1.second)) {
    pair<int,int> overlap(r2.first, r2.second < r1.second ? r2.second : r1.second);
    return pair<pair<int,int>, bool>(overlap, true);
  }

  return pair<pair<int,int>, bool>(pair<int,int>(-1,-1), false);
}

class Step {
  public: static const int INIT_PROC_MIN_LIMIT = -50;
  public: static const int INIT_PROC_MAX_LIMIT = 50;

  private:
    const bool state;
    const int x_min, x_max, y_min, y_max, z_min, z_max;

    static const bool NumWithinInitProcLimits(const int& a) { return IntWithinRange(a, Step::INIT_PROC_MIN_LIMIT, Step::INIT_PROC_MAX_LIMIT); }

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

class Cuboid {
  private:
    pair<int,int> x_range;
    pair<int,int> y_range;
    pair<int,int> z_range;

  public:
    Cuboid(const pair<int,int>& x, const pair<int,int>& y, const pair<int,int>& z) {
      if (x.first > x.second || y.first > y.second || z.first > z.second) {
        throw invalid_argument("cuboid range start cannot be greater than range end");
      }

      this->x_range = x;
      this->y_range = y;
      this->z_range = z;
    }

    const pair<int,int> GetXRange() const { return this->x_range; }
    const pair<int,int> GetYRange() const { return this->y_range; }
    const pair<int,int> GetZRange() const { return this->z_range; }
    const unsigned long long int Volume() const {
      return (unsigned long long int)(this->x_range.second-this->x_range.first+1) * (unsigned long long int)(this->y_range.second-this->y_range.first+1) * (unsigned long long int)(this->z_range.second-this->z_range.first+1);
    }

    // Overlap returns NULL if there is no overlap between this Cuboid and 'other'
    // Otherwise, it returns the area of overlap as a Cuboid
    Cuboid* Overlap(const Cuboid* other) const {
      pair<pair<int,int>, bool> x_overlap = RangeOverlap(this->x_range, other->x_range);
      if (!x_overlap.second) {
        return NULL;
      }

      pair<pair<int,int>, bool> y_overlap = RangeOverlap(this->y_range, other->y_range);
      if (!y_overlap.second) {
        return NULL;
      }

      pair<pair<int,int>, bool> z_overlap = RangeOverlap(this->z_range, other->z_range);
      if (!z_overlap.second) {
        return NULL;
      }

      return new Cuboid(x_overlap.first, y_overlap.first, z_overlap.first);
    }

    vector<Cuboid*> Subtract(Cuboid* c) const;

    bool operator<(const Cuboid& other) const { // necessary to implement for to use this type as key in map
      if (this->x_range.first < other.x_range.first) { return true; }
      else if (this->x_range.first == other.x_range.first) {
        if (this->x_range.second < other.x_range.second) { return true; }
        else if (this->x_range.second == other.x_range.second) {
          if (this->y_range.first < other.y_range.first) { return true; }
          else if (this->y_range.first == other.y_range.first) {
            if (this->y_range.second < other.y_range.second) { return true; }
            else if (this->y_range.second == other.y_range.second) {
              if (this->z_range.first < other.z_range.first) { return true; }
              else if (this->z_range.first == other.z_range.first) {
                return this->z_range.second < other.z_range.second;
              }
            }
          }
        }
      }

      return false;
    }

    bool operator==(const Cuboid& other) const {
      return (this->x_range == other.x_range) && (this->y_range == other.y_range) && (this->z_range == other.z_range);
    }

    ~Cuboid() {}
};

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const Cuboid& c) {
  os << "Cuboid: x=" << c.GetXRange().first << ".." << c.GetXRange().second << ",y=" << c.GetYRange().first << ".." << c.GetYRange().second << ",z=" << c.GetZRange().first << ".." << c.GetZRange().second;
  return os;
}

// Subtract will return this cuboid divided into parts to exclude all cubes in c, returning the list of parts as Cuboid objects
vector<Cuboid*> Cuboid::Subtract(Cuboid* c) const {
  vector<Cuboid*> parts;

  // Cover cubes to the -x direction of c
  if (this->x_range.first < c->x_range.first) {
    Cuboid* left_block = new Cuboid(
      pair<int,int>(this->x_range.first, c->x_range.first-1),
      this->y_range,
      this->z_range);

    parts.push_back(left_block);
  }

  // Cover cubes in the same x range as c:
  // cover cubes to the -y direction of c
  if (this->y_range.first < c->y_range.first) {
    Cuboid* skinny_under_block = new Cuboid(
      c->x_range,
      pair<int,int>(this->y_range.first, c->y_range.first-1),
      this->z_range);

    parts.push_back(skinny_under_block);
  }

  // cover cubes in the same x, y ranges as c:
  // cover cubes in the -z direction of c
  if (this->z_range.first < c->z_range.first) {
    Cuboid* tiny_behind_block = new Cuboid(
      c->x_range,
      c->y_range,
      pair<int,int>(this->z_range.first, c->z_range.first-1));

    parts.push_back(tiny_behind_block);
  }
  // cubes in the same x,y,z ranges as c are in c; not to be covered
  // cover cubes in the +z direction of c
  if (this->z_range.second > c->z_range.second) {
    Cuboid* tiny_infront_block = new Cuboid(
      c->x_range,
      c->y_range,
      pair<int,int>(c->z_range.second+1, this->z_range.second));

    parts.push_back(tiny_infront_block);
  }

  // cover cubes in the +y direction of c
  if (this->y_range.second > c->y_range.second) {
    Cuboid* skinny_above_block = new Cuboid(
      c->x_range,
      pair<int,int>(c->y_range.second+1, this->y_range.second),
      this->z_range);

    parts.push_back(skinny_above_block);
  }

  // Cover cubes to the +x direction of c
  if (this->x_range.second > c->x_range.second) {
    Cuboid* right_block = new Cuboid(
      pair<int,int>(c->x_range.second+1, this->x_range.second),
      this->y_range,
      this->z_range);

    parts.push_back(right_block);
  }

  return parts;
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
  // Store "on" cuboids rather than each coordinate, which are too numerous to work well with
  map<Cuboid*,int> on_grid; // value is merely a placeholder

  for (auto s:steps) {
    queue<Cuboid*> changed_cuboids;
    changed_cuboids.push(new Cuboid(s.GetXRange(), s.GetYRange(), s.GetZRange()));

    // Process all cuboids covered by this change
    while (!changed_cuboids.empty()) {
      Cuboid* change = changed_cuboids.front();
      changed_cuboids.pop();

      bool change_is_unique = true; // until proven false

      for (auto record:on_grid) {
        Cuboid* existing_cuboid = record.first;
        
        // If this cuboid intersects with an existing recorded cuboid ...
        Cuboid* overlap = existing_cuboid->Overlap(change);
        if (overlap == NULL) {
          continue;
        }
        else {
          change_is_unique = false;
        }

        // ... divvy up the new cuboid according to overlap (easier to manipulate before storing, rather than manipulating already-recorded cuboids)
        if (s.GetState()) { // turning cubes "on"; things will be added to the map
          if (!(*overlap == *change)) {
            // change is partially covered by this existing record; divvy the remaining cubes into chunks to check against the rest of the grid
            vector<Cuboid*> non_overlapping_changed_parts = change->Subtract(overlap);
            for (auto p:non_overlapping_changed_parts) {
              changed_cuboids.push(p);
            }
          }

          delete overlap;
          break;
        }
        else { // turning cubes "off"; things will be subtracted from the map
          on_grid.erase(existing_cuboid);

          if (!(*overlap == *existing_cuboid)) {
            vector<Cuboid*> non_overlapping_existing_cuboid_parts = existing_cuboid->Subtract(overlap);
            for (auto p:non_overlapping_existing_cuboid_parts) {
              on_grid[p] = 1;
            }
          }

          delete existing_cuboid;
          delete overlap;
          break;
        }
      }

      if (change_is_unique && s.GetState()) {
        on_grid[change] = 1;
      }
      else if (!change_is_unique && !s.GetState()) {
        changed_cuboids.push(change);
      }
      else {
        delete change;
      }
    }
  }

  unsigned long long int on_count2 = 0;
  for (auto c:on_grid) {
    on_count2 += c.first->Volume();
  }

  cout << "Part 2 answer: " << on_count2 << endl;

  // Clean up memory
  for (auto c:on_grid) {
    delete c.first;
  }

  return 0;
}
