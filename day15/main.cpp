// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 15 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the lowest total risk of any path from the top left to the bottom right?
// Part 2: Using the full map, what is the lowest total risk of any path from the top left to the bottom right?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <queue>
#include <map>

using namespace std;

// Use this type declaration to facilitate use of pair type for coordinates
typedef pair<int,int> node;

typedef int (*heuristic_func)(const node& point, const node& end);

// AStarHeuristic estimates the cost of reaching the given end coordinate from the given point
// Since diagonal moves are not possible, the estimated cost is moving directly across rows and directly across columns, since a move costs at least 1
// The end point is expected to be at the max row and max col values, but do not assume (it costs just as much to move left as right)
int AStarHeuristic(const node& point, const node& end) {
  return abs(0.0 + end.first - point.first) + abs(0.0 + end.second - point.second);
}

// FScoreCoordinate is used to represent nodes for the A* algorithm, so they can be sorted in a priority queue by fScores
class FScoreCoordinate {
  public:
    FScoreCoordinate(const int& score, const node& coord) : f_score(score), coordinate(coord) {}
    int f_score;
    node coordinate;

    FScoreCoordinate& operator=(const FScoreCoordinate& other); // implement assignment operator to work with priority queue
};

FScoreCoordinate& FScoreCoordinate::operator=(const FScoreCoordinate& other) {
  if (this == &other) {
    return *this;
  }

  this->f_score = other.f_score;
  this->coordinate = node(other.coordinate.first, other.coordinate.second);
  return *this;
}

// FScoreCoordinateComparison is used to represent nodes for the A* algorithm, so they can be sorted in a priority queue by fScores
class FScoreCoordinateComparison {
  public:
    bool operator() (const FScoreCoordinate& l, const FScoreCoordinate& r) const {
      // want this comparator to act for a min-queue on fScore
      return l.f_score > r.f_score;
    }
};

vector<node> DeterminePath(const map<node,node>& came_from, const node& last_node) {
  node current_node = last_node;
  vector<node> path(1, current_node);
  while (came_from.find(current_node) != came_from.end()) {
    auto it = came_from.find(current_node);
    current_node = it->second;
    path.insert(path.begin(), current_node);
  }

  return path;
}

// AdjacentNodes returns a list of coordinates of the adjacent nodes to n
// For Part 1, this is neighbords immediately above, below, left, and right of n
// If n is on the edge of the map, the returned listed will be short (only valid map coordinates are returned)
vector<node> AdjacentNodes(const node& n, const vector<vector<int>>& map) {
  vector<node> candidates;
  candidates.push_back(node(n.first-1,n.second));
  candidates.push_back(node(n.first+1,n.second));
  candidates.push_back(node(n.first,n.second-1));
  candidates.push_back(node(n.first,n.second+1));

  vector<node> final;
  for (auto c:candidates) {
    if (c.first < 0 || c.first >= map.size()) {
      continue;
    }
    if (c.second < 0 || c.second >= map[c.first].size()) {
      continue;
    }

    final.push_back(c);
  }

  return final;
}

// AStar implements the A* path-finding algorithm; developed using pseudocode from https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
// If I use a heuristic function like h(node) = node.x + node.y (since diagonals cannot be used): AStarHeuristic, I believe that means h() is monotone/consistent and we should have an optimal performance over Dijkstra's
vector<node> AStar(
  const node& start,
  const node& end,
  const vector<vector<int>>& node_map,
  heuristic_func h
) {
  // Create queue tracking discovered nodes set to be considered, with their fScores
  priority_queue<FScoreCoordinate, vector<FScoreCoordinate>, FScoreCoordinateComparison> open_set;
  int f_score_start = (*h)(start,end);
  open_set.push(FScoreCoordinate(f_score_start, start));

  // Create map tracking the directionality of the lowest risk path from start
  map<node,node> came_from;

  // Create map tracking the value of the lowest risk path from start to other nodes
  map<node,int> g_score;
  g_score[start] = 0;

  while (!open_set.empty()) {
    FScoreCoordinate curr = open_set.top(); // node to consider with the lowest fScore
    if (curr.coordinate == end) {
      return DeterminePath(came_from, curr.coordinate);
    }

    open_set.pop(); // remove curr from consideration for the time being
    vector<node> adjacent_coords = AdjacentNodes(curr.coordinate, node_map);
    for (auto a:adjacent_coords) {
      int tentative_g_score = g_score[curr.coordinate] + node_map[a.first][a.second]; // cost of moving to the adjacent node is equal to that node's risk level
      // If the adjacent node does not have a definitive gScore yet, the default is infinity
      if (g_score.find(a) == g_score.end() || tentative_g_score < g_score[a]) {
        // this adjacent node is the best option for curr to travel to
        came_from[a] = curr.coordinate;
        g_score[a] = tentative_g_score;

        int f_score = tentative_g_score + (*h)(a, end);
        // "With a consistent heuristic, A* is guaranteed to find an optimal path without processing any node more than once" - so we will not check if the adjacent node is already in open_set before adding it, which would necessitate storing all nodes in open_set in a separate structure
        open_set.push(FScoreCoordinate(f_score, a));
      }
    }
  }

  return vector<node>(); // failure case
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day15/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  vector<vector<int>> risk_map; // accessed like [row][col]

  risk_map.resize(file_results.first.size());
  for (int row = 0; row < file_results.first.size(); ++row) {
    risk_map[row].resize(file_results.first[row].size());
    for (int col = 0; col < file_results.first[row].size(); ++col) {
      int i;
      try {
        i = stoi(file_results.first[row].substr(col, 1));
      }
      catch (invalid_argument& ia) {
        cout << "File value at row " << row << ", column " << col << " is not a valid int" << endl;
        return -1;
      }
      risk_map[row][col] = i;
    }
  }

  // Part 1:
  // all pairs are represented as (row,col)
  vector<node> lowest_risk_path = AStar(
    node(0,0),
    node(risk_map.size()-1,risk_map[risk_map.size()-1].size()-1),
    risk_map,
    &AStarHeuristic);
  if (lowest_risk_path.size() <= 0) {
    cout << "Algorithm failed to find a path from start to end" << endl;
    return -1;
  }

  int total_risk = 0;
  for (int i=1; i < lowest_risk_path.size(); ++i) {
    total_risk += risk_map[lowest_risk_path[i].first][lowest_risk_path[i].second];
  }

  cout << "Part 1 answer: " << total_risk << endl;

  // Part 2:
  // Expand map
  const int grow_factor = 5;
  int initial_map_rows = risk_map.size();
  int initial_map_cols = risk_map[0].size();

  // resize map
  risk_map.resize(initial_map_rows * grow_factor);
  for (int row = 0; row < risk_map.size(); ++row) {
    risk_map[row].resize(initial_map_cols * grow_factor);
  }

  // fill in further values
  for (int row = 0; row < risk_map.size(); ++row) {
    for (int col = 0; col < risk_map[row].size(); ++col) {
      if (row < initial_map_rows && col < initial_map_cols) {
        continue; // the original values don't change
      }

      // since the values are getting filled out on a single row, left to right, copy from the left then from above
      int existing_val;
      if (row - initial_map_rows < 0) {
        // copy from the columns to the left
        existing_val = risk_map[row][col - initial_map_cols];
      }
      else {
        // copy from above since it's already been filled in
        existing_val = risk_map[row - initial_map_rows][col];
      }

      risk_map[row][col] = existing_val == 9 ? 1 : existing_val + 1;
    }
  }

  // Run the algorithm again
  lowest_risk_path = AStar(
    node(0,0),
    node(risk_map.size()-1,risk_map[risk_map.size()-1].size()-1),
    risk_map,
    &AStarHeuristic);
  if (lowest_risk_path.size() <= 0) {
    cout << "Algorithm failed to find a path from start to end" << endl;
    return -1;
  }

  total_risk = 0;
  for (int i=1; i < lowest_risk_path.size(); ++i) {
    total_risk += risk_map[lowest_risk_path[i].first][lowest_risk_path[i].second];
  }

  cout << "Part 2 answer: " << total_risk << endl;

  return 0;
}
