// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 19 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: How many beacons are there?
// Part 2: What is the largest Manhattan distance between any two scanners?

#include "../util/fileutil.hpp" // ReadLinesFromFile,ParseSeparatedInts
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <cmath>

using namespace std;

class CoordinateDistance; // declare above so the Coordinate class can use the type

class Coordinate {
  public:
    int x; 
    int y;
    int z;

    Coordinate(): x(-1), y(-1), z(-1) {} // define for default constructor in vector<Coordinate> resizing
    Coordinate(const int& a, const int& b, const int& c): x(a), y(b), z(c) {}

    bool operator<(const Coordinate& other) const { // necessary to implement for map usage
      if (this->x < other.x) {
        return true;
      }
      else if (this->x == other.x) {
        if (this->y < other.y) {
          return true;
        }
        else if (this->y == other.y) {
          return this->z < other.z;
        }
      }

      return false;
    }

    bool operator==(const Coordinate& other) const { // necessary to specify for algorithm implementation
      return (this->x == other.x && this->y == other.y && this->z == other.z);
    }

    void Translate(const CoordinateDistance& d);

    ~Coordinate() {} // default destructor works
};

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const Coordinate& c) {
  os << "(" << c.x << "," << c.y << "," << c.z << ")";
  return os;
}

// declaring the rotation function type to make the code easier
typedef Coordinate (*rotate_func)(const Coordinate& c);

// Thinking about this orientation as (to-the-left, up, forward) from the scanner's point of view
Coordinate Original(const Coordinate& c) {
  Coordinate ret(c.x, c.y, c.z);
  return ret;
}

Coordinate Rotate_1(const Coordinate& c) {
  Coordinate ret(-1 * c.y, c.z, -1 * c.x);
  return ret;
}

Coordinate Rotate_2(const Coordinate& c) {
  Coordinate ret(-1 * c.z, c.x, -1 * c.y);
  return ret;
}

Coordinate Rotate_3(const Coordinate& c) {
  Coordinate ret(c.x, -1 * c.y, -1 * c.z);
  return ret;
}

Coordinate Rotate_4(const Coordinate& c) {
  Coordinate ret(c.y, -1 * c.z, -1 * c.x);
  return ret;
}

Coordinate Rotate_5(const Coordinate& c) {
  Coordinate ret(c.z, -1 * c.x, -1 * c.y);
  return ret;
}

Coordinate Rotate_6(const Coordinate& c) {
  Coordinate ret(-1 * c.x, c.y, -1 * c.z);
  return ret;
}

Coordinate Rotate_7(const Coordinate& c) {
  Coordinate ret(c.y, c.z, c.x);
  return ret;
}

Coordinate Rotate_8(const Coordinate& c) {
  Coordinate ret(c.z, c.x, c.y);
  return ret;
}

Coordinate Rotate_9(const Coordinate& c) {
  Coordinate ret(-1 * c.x, -1 * c.y, c.z);
  return ret;
}

Coordinate Rotate_10(const Coordinate& c) {
  Coordinate ret(c.z, -1 * c.y, c.x);
  return ret;
}

Coordinate Rotate_11(const Coordinate& c) {
  Coordinate ret(c.x, -1 * c.z, c.y);
  return ret;
}

Coordinate Rotate_12(const Coordinate& c) {
  Coordinate ret(-1 * c.y, c.x, c.z);
  return ret;
}

Coordinate Rotate_13(const Coordinate& c) {
  Coordinate ret(c.z, c.y, -1 * c.x);
  return ret;
}

Coordinate Rotate_14(const Coordinate& c) {
  Coordinate ret(c.x, c.z, -1 * c.y);
  return ret;
}

Coordinate Rotate_15(const Coordinate& c) {
  Coordinate ret(-1 * c.y, -1 * c.x, -1 * c.z);
  return ret;
}

Coordinate Rotate_16(const Coordinate& c) {
  Coordinate ret(-1 * c.z, -1 * c.y, -1 * c.x);
  return ret;
}

Coordinate Rotate_17(const Coordinate& c) {
  Coordinate ret(-1 * c.x, -1 * c.z, -1 * c.y);
  return ret;
}

Coordinate Rotate_18(const Coordinate& c) {
  Coordinate ret(c.y, c.x, -1 * c.z);
  return ret;
}

Coordinate Rotate_19(const Coordinate& c) {
  Coordinate ret(-1 * c.z, c.y, c.x);
  return ret;
}

Coordinate Rotate_20(const Coordinate& c) {
  Coordinate ret(-1 * c.x, c.z, c.y);
  return ret;
}

Coordinate Rotate_21(const Coordinate& c) {
  Coordinate ret(c.y, -1 * c.x, c.z);
  return ret;
}

Coordinate Rotate_22(const Coordinate& c) {
  Coordinate ret(-1 * c.y, -1 * c.z, c.x);
  return ret;
}

Coordinate Rotate_23(const Coordinate& c) {
  Coordinate ret(-1 * c.z, -1 * c.x, c.y);
  return ret;
}

class CoordinateDistance {
  public:
    const int x_diff; 
    const int y_diff;
    const int z_diff;

    // from a's perspective, distance to b
    CoordinateDistance() : // define default constructor for ease
      x_diff(-1),
      y_diff(-1),
      z_diff(-1) {}
    CoordinateDistance(const Coordinate& a, const Coordinate& b) :
      x_diff(b.x - a.x),
      y_diff(b.y - a.y),
      z_diff(b.z - a.z) {}

    bool operator<(const CoordinateDistance& other) const { // necessary to implement for map usage
      if (this->x_diff < other.x_diff) {
        return true;
      }
      else if (this->x_diff == other.x_diff) {
        if (this->y_diff < other.y_diff) {
          return true;
        }
        else if (this->y_diff == other.y_diff) {
          return this->z_diff < other.z_diff;
        }
      }

      return false;
    }
    double ManhattanDistance() const {
      return abs(this->x_diff) + abs(this->y_diff) + abs(this->z_diff);
    }

    ~CoordinateDistance() {} // default destructor works
};

void Coordinate::Translate(const CoordinateDistance& d) {
  this->x += d.x_diff;
  this->y += d.y_diff;
  this->z += d.z_diff;
}

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const CoordinateDistance& d) {
  os << "Distance [" << d.x_diff << "," << d.y_diff << "," << d.z_diff << "]";
  return os;
}

class ScannerBeaconSet {
  private:
    vector<Coordinate> beacons;
    Coordinate scanner;

  public:
    ScannerBeaconSet(const vector<Coordinate>& beacon_set) {
      this->beacons.resize(beacon_set.size());
      for (int i = 0; i < beacon_set.size(); ++i) {
        this->beacons[i] = beacon_set[i];
      }

      this->scanner = Coordinate(0,0,0);
    }


    const vector<Coordinate>& GetBeacons() const { return this->beacons; }
    const Coordinate GetScanner() const { return this->scanner; }
    void ApplyTransformation(const rotate_func& rotate, const CoordinateDistance& translate);

    ~ScannerBeaconSet() {} // default destructor works fine
};

void ScannerBeaconSet::ApplyTransformation(
  const rotate_func& rotate,
  const CoordinateDistance& translate)
{
  // Rotate and translate beacons
  for (auto it = this->beacons.begin(); it != this->beacons.end(); ++it) {
    Coordinate transformed_b = rotate(*it);
    transformed_b.Translate(translate);
    *it = transformed_b;
  }

  // Rotate and translate scanner
  Coordinate transformed_s = rotate(this->scanner);
  transformed_s.Translate(translate);
  this->scanner = transformed_s;
}

class Universe {
  private:
    // maps a beacon in the Universe to its distance to every other beacon in the Universe
    map<Coordinate, map<CoordinateDistance,Coordinate>> beacon_graph;
    vector<Coordinate> scanners;

  public:
    Universe(ScannerBeaconSet* scanner_set);

    static vector<rotate_func> PossibleRotations() {
      vector<rotate_func> ret;
      ret.resize(24);
      ret[0] = &Original;
      ret[1] = &Rotate_1;
      ret[2] = &Rotate_2;
      ret[3] = &Rotate_3;
      ret[4] = &Rotate_4;
      ret[5] = &Rotate_5;
      ret[6] = &Rotate_6;
      ret[7] = &Rotate_7;
      ret[8] = &Rotate_8;
      ret[9] = &Rotate_9;
      ret[10] = &Rotate_10;
      ret[11] = &Rotate_11;
      ret[12] = &Rotate_12;
      ret[13] = &Rotate_13;
      ret[14] = &Rotate_14;
      ret[15] = &Rotate_15;
      ret[16] = &Rotate_16;
      ret[17] = &Rotate_17;
      ret[18] = &Rotate_18;
      ret[19] = &Rotate_19;
      ret[20] = &Rotate_20;
      ret[21] = &Rotate_21;
      ret[22] = &Rotate_22;
      ret[23] = &Rotate_23;

      return ret;
    }

    const vector<Coordinate> GetBeacons() const {
      vector<Coordinate> ret;
      for (auto b:this->beacon_graph) {
        ret.push_back(b.first);
      }
      return ret;
    }
    int BeaconCount() const { return this->beacon_graph.size(); }
    const vector<Coordinate>& GetScanners() const { return this->scanners; }
    int DistanceMatchCount(const Coordinate& space_beacon, const map<CoordinateDistance,Coordinate>& other_distances) const;
    void Print() const;
    void Merge(const ScannerBeaconSet* to_merge);

    ~Universe() {} // default destructor works fine
};

map<Coordinate, map<CoordinateDistance,Coordinate>> CreateCompleteDistanceGraphFromNodes(const vector<Coordinate>& nodes) {
  map<Coordinate, map<CoordinateDistance,Coordinate>> graph;

  // Populate the graph with the nodes
  for (auto n:nodes) {
    map<CoordinateDistance,Coordinate> n_distances;

    // Calculate the distance between each node
    for (auto other:nodes) {
      if (n == other) { continue; } // do not compute distance between node and itself
      CoordinateDistance dist(n, other);
      n_distances[dist] = other;
    }

    graph[n] = n_distances;
  }

  return graph;
}

Universe::Universe(ScannerBeaconSet* scanner_set) {
  this->beacon_graph = CreateCompleteDistanceGraphFromNodes(scanner_set->GetBeacons());

  this->scanners.resize(1);
  this->scanners[0] = scanner_set->GetScanner();
}

// DistanceMatchCount returns the number of CoordinateDistance keys that are in other_distances as well as the universe's beacon graph at the specified universe beacon
int Universe::DistanceMatchCount(
  const Coordinate& space_beacon,
  const map<CoordinateDistance,Coordinate>& other_distances) const
{
  auto space_beacon_graph = this->beacon_graph.find(space_beacon);
  if (space_beacon_graph == this->beacon_graph.end()) {
    cout << "The coordinate provided as the beacon is not in the universe's graph" << endl;
    return -1;
  }

  int match_count = 0;
  for (auto d:other_distances) { // iterate over the given distances, since it is expected to be a shorter list
    if (space_beacon_graph->second.find(d.first) != space_beacon_graph->second.end()) {
      ++match_count;
    }
  }

  return match_count;
}

// helper function for debugging
void Universe::Print() const {
  cout << "Beacons and distances:" << endl;
  for (auto beacon_record:this->beacon_graph) {
    cout << beacon_record.first << endl;
    for (auto distances:beacon_record.second) {
      cout << "\t" << distances.first << " -> " << distances.second << endl;
    }
  }
  cout << endl;
}

void Universe::Merge(const ScannerBeaconSet* to_merge) {
  // Combine beacon lists from the current state of the universe, and the given scanner set
  vector<Coordinate> all_beacons;
  all_beacons.resize(this->beacon_graph.size());
  auto git = this->beacon_graph.cbegin();
  for (int i = 0; i < all_beacons.size() && git != this->beacon_graph.cend(); ++i,++git) {
    all_beacons[i] = git->first;
  }

  all_beacons.insert(all_beacons.end(), to_merge->GetBeacons().cbegin(), to_merge->GetBeacons().cend());

  // Re-compute the complete graph
  // TODO optimization: only compute distances for new values in old beacons
  this->beacon_graph = CreateCompleteDistanceGraphFromNodes(all_beacons);

  // Incorporate scanner set's scanner coordinate
  this->scanners.push_back(to_merge->GetScanner());
}

// FindTransformationForOverlap determines if there are 12+ overlapping beacons between the given set and universe
// Return values are the rotation function and translation needed on the original/rotated beacons to match those in the given universe
tuple<rotate_func,CoordinateDistance,int> FindTransformationForOverlap(
  const vector<Coordinate>& beacons_to_match,
  Universe* universe)
{
  vector<Coordinate> known_beacons = universe->GetBeacons();

  for (auto r:Universe::PossibleRotations()) {
    // Transform beacons in the set to match according to rotation
    vector<Coordinate> rotated_beacons;
    rotated_beacons.resize(beacons_to_match.size());
    for (int i = 0; i < rotated_beacons.size(); ++i) {
      rotated_beacons[i] = r(beacons_to_match[i]);
    }

    // Compute the complete graph of the beacons in the set to match
    map<Coordinate, map<CoordinateDistance,Coordinate>> rotated_complete_graph = CreateCompleteDistanceGraphFromNodes(rotated_beacons);

    // Try a translation of each beacon-to-match onto each known beacon in the universe to see if it produces a successful match
    for (auto b:rotated_beacons) {
      for (auto space_b:known_beacons) {
        int matches = universe->DistanceMatchCount(space_b, rotated_complete_graph[b]);
        // If we get >=12 matches (with the rotated beacon matching the universe's beacon), we can merge the set into the universe
        if (matches >= 11) {
          return tuple<rotate_func,CoordinateDistance,int>(r, CoordinateDistance(b, space_b), 0);
        }
      }
    }
  }

  return tuple<rotate_func,CoordinateDistance,int>(NULL, CoordinateDistance(), -1);
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day19/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input:
  queue<ScannerBeaconSet*> unmerged_scanners;

  // parse the scanners as unmerged beacon sets
  auto line = file_results.first.begin();
  while (line != file_results.first.end()) {
    if (line->size() <= 0) {
      ++line; // move past the blank line before scanner x beacons
    }

    ++line; // move past the scanner number line

    vector<Coordinate> scanner_beacons;

    // parse each beacon coordinate
    while (line != file_results.first.end() && line->size() > 0) {
      pair<vector<int>, int> line_ints = ParseSeparatedInts(*line, ",");
      if (line_ints.second < 0 || line_ints.first.size() != 3) {
        cout << "Unexpected beacon line format: " << *line << endl;
        return -1;
      }

      Coordinate beacon(line_ints.first[0], line_ints.first[1], line_ints.first[2]);
      scanner_beacons.push_back(beacon);

      ++line; // move to next coordinate
    }

    unmerged_scanners.push(new ScannerBeaconSet(scanner_beacons));
  }

  // Set full_space: (the scanner-set to match against) equal to a single scanner
  int scanner_to_make_initial_universe = 0;
  int scanners_popped = 0;
  while (scanners_popped < scanners_popped) {
    auto s = unmerged_scanners.front();
    unmerged_scanners.pop();
    unmerged_scanners.push(s);
    ++scanners_popped;
  }

  ScannerBeaconSet* init_universe = unmerged_scanners.front();
  unmerged_scanners.pop();
  Universe* full_space = new Universe(init_universe);
  delete init_universe;

  // Part 1:
  // While the list of remaining unmatched scanners is not empty:
  while (!unmerged_scanners.empty()) {
    ScannerBeaconSet* to_match = unmerged_scanners.front();
    unmerged_scanners.pop();

    vector<Coordinate> original_beacons = to_match->GetBeacons();

    tuple<rotate_func,CoordinateDistance,int> transformation_result = FindTransformationForOverlap(original_beacons, full_space);
    
    // Check if an overlap is valid at the moment
    if (get<2>(transformation_result) < 0) {
      // no overlap has been found; add it to the back of the scanner sets to try
      unmerged_scanners.push(to_match);
      continue;
    }

    // Apply the transformation needed for the overlap
    to_match->ApplyTransformation(get<0>(transformation_result), get<1>(transformation_result));

    // Merge the scanner set into the universe
    full_space->Merge(to_match);

    // Delete the original scanner set now that it's been merged
    delete to_match;
  }

  cout << "Part 1 answer: " << full_space->BeaconCount() << endl; // took most of 2m32s to run on my machine

  // Part 2:
  vector<Coordinate> final_scanners = full_space->GetScanners();

  double max_distance = 0;
  for (auto a:final_scanners) {
    for (auto b:final_scanners) {
      if (a == b) { continue; } // no need to compute distance to self

      CoordinateDistance diff(a, b);
      double distance = diff.ManhattanDistance();
      if (distance > max_distance) {
        max_distance = distance;
      }
    }
  }

  cout << "Part 2 answer: " << max_distance << endl;

  // Clean up memory
  delete full_space;
  // scanner sets have been cleaned up by processing

  return 0;
}
