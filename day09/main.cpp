// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 9 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the sum of the risk levels of all low points on your heightmap?
// Part 2: What do you get if you multiply together the sizes of the three largest basins?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <queue>

using namespace std;

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day09/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  vector<vector<pair<int,bool>>> height_map; // accessed by [row][col]; pair represents height and whether it's been explored
  for (auto line:file_results.first) {
    vector<pair<int,bool>> line_heights;
    line_heights.resize(line.size());
    for (int i = 0; i < line.size(); ++i) {
      line_heights[i].first = line[i] - '0'; // this evaluates to an int representing the number in the char
      line_heights[i].second = false; // mark everything as unexplored to start
    }

    height_map.push_back(line_heights);
  }

  // Part 1:
  // Sum up risk levels
  int risk_sum = 0;
  // Collect all low points to enable Part 2
  vector<pair<int,int>> low_points;
  for (int row = 0; row < height_map.size(); ++row) {
    for (int col = 0; col < height_map[row].size(); ++col) {
      int curr = height_map[row][col].first;
      if (row-1 >= 0 && curr >= height_map[row-1][col].first) {
        continue;
      }
      if (row+1 < height_map.size() && curr >= height_map[row+1][col].first) {
        continue;
      }
      if (col-1 >= 0 && curr >= height_map[row][col-1].first) {
        continue;
      }
      if (col+1 < height_map[row].size() && curr >= height_map[row][col+1].first) {
        continue;
      }

      low_points.push_back(pair<int,int>(row,col));
      risk_sum += curr + 1; // The risk level of a low point is 1 plus its height
    }
  }

  cout << "Part 1 answer: " << risk_sum << endl;

  // Part 2:
  // Find all elements in a basin, starting with its low point
  // Use a priority queue to collect all basin sizes
  priority_queue<int> basin_sizes;
  for (auto low_point:low_points) {
    int size = 0;
    // Do a breadth-first search of the basin; we need a way to see if we've visited a spot already, so we don't double-count a point for the basin while searching for its edge
    
    // set up the exploring queue
    queue<pair<int,int>> q; // represents (row,col) pairs
    // label the root of the search (the low point) as explored
    height_map[low_point.first][low_point.second].second = true;
    // enqueue root
    q.push(pair<int,int>(low_point.first, low_point.second));

    // while the exploring queue is not empty...
    while (q.size() > 0) {
      // dequeue a point to consider
      pair<int,int> p = q.front();
      q.pop();
      int row = p.first, col = p.second;
      
      // if the point is a 9, it is the edge of the basin and doesn't count toward its size; continue with the algorithm
      if (height_map[row][col].first == 9) {
        continue;
      }
      else {
        size += 1;
      }

      // for all horizontal and vertical neighbors of the point which are within the bounds of the map and unexplored...
      if (row-1 >= 0 && !height_map[row-1][col].second) {
        // ... label the neighbor as explored
        height_map[row-1][col].second = true;
        // ... enqueue the neightbor
        q.push(pair<int,int>(row-1,col));
      }
      if (row+1 < height_map.size() && !height_map[row+1][col].second) {
        // ... label the neighbor as explored
        height_map[row+1][col].second = true;
        // ... enqueue the neightbor
        q.push(pair<int,int>(row+1,col));
      }
      if (col-1 >= 0 && !height_map[row][col-1].second) {
        // ... label the neighbor as explored
        height_map[row][col-1].second = true;
        // ... enqueue the neightbor
        q.push(pair<int,int>(row,col-1));
      }
      if (col+1 < height_map[row].size() && !height_map[row][col+1].second) {
        // ... label the neighbor as explored
        height_map[row][col+1].second = true;
        // ... enqueue the neightbor
        q.push(pair<int,int>(row,col+1));
      }
    }

    basin_sizes.push(size);
  }

  // Find the 3 largest basins
  int s0 = basin_sizes.top();
  basin_sizes.pop();
  int s1 = basin_sizes.top();
  basin_sizes.pop();
  int s2 = basin_sizes.top();
  basin_sizes.pop();
  cout << "Part 2 answer: " << s0 * s1 * s2 << endl;

  return 0;
}
