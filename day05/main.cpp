// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 5 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: Consider only horizontal and vertical lines. At how many points do at least two lines overlap?
// Part 2: Consider all of the lines (including the diagonal lines at 45 degrees). At how many points do at least two lines overlap?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <regex>
#include <string>
#include <cmath>

using namespace std;

// O(1)
bool IsHorizontal(pair<pair<int,int>, pair<int,int>> line_segment) {
  if (line_segment.first.second == line_segment.second.second) { // same y values: horizontal
    return true;
  }

  return false;
}

// O(1)
bool IsVertical(pair<pair<int,int>, pair<int,int>> line_segment) {
  if (line_segment.first.first == line_segment.second.first) { // same x values: vertical
    return true;
  }

  return false;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day05/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Determine size of map
  int max_height = -1;
  int max_width = -1;

  // Parse file results into pairs of pairs of coordinates
  vector<pair<pair<int,int>, pair<int,int>>> line_segments;
  regex r("(\\d+),(\\d+) -> (\\d+),(\\d+)");
  for (vector<string>::const_iterator file_line = file_results.first.begin(); file_line != file_results.first.end(); ++file_line) {
    smatch m;
    regex_search(*file_line, m, r);
    if (m.size() != 5) {
      cout << "Unexpected format of line: '" << *file_line << "'" << endl;
      return -1;
    }

    int x1 = stoi(m[1]), y1 = stoi(m[2]), x2 = stoi(m[3]), y2 = stoi(m[4]);
    // Check if we need to adjust our map size
    if (x1 > max_width) {
      max_width = x1;
    }
    if (x2 > max_width) {
      max_width = x2;
    }
    if (y1 > max_height) {
      max_height = y1;
    }
    if (y2 > max_height) {
      max_height = y2;
    }

    // Store the line segment
    pair<pair<int,int>, pair<int,int>> line_segment(pair<int,int>(x1, y1), pair<int,int>(x2, y2));
    line_segments.push_back(line_segment);
  }

  if (max_height == -1 || max_width == -1) {
    cout << "Map size not determined" << endl;
    return -1;
  }

  // Design
  // option 1: keep track of the whole map, and what points are covered (vector of vector of ints representing number of lines covering the points) - this is constant-time access to fill in the map, so O(num_lines*max(map_width, map_height)) to fill all, but O(map_height*map_width) to finally check all the points - this could be optimized by keeping a counter while filling up the map, incrementing the counter when a map point goes from 1->2, O(1) -> O(num_lines*max(map_width, map_height))
  // option 2: check each pair of line segments against each other; if all horizontals were ordered by their y position, perhaps using a priority queue, it would be fast to check if two horizontal lines overlapped at the same y, because you know two horizontals cannot overlap if they have different y values (similarly with verticals), all the while keeping a list of qualifying intersection points; then all horizontals would have to be checked against the verticals with relevant y values in their range, and vice versa...
  // Given that I'm guessing Part 2 of the puzzle will be incorporating the diagonal line segments as well, or else plotting a path thru the map, I will go with Option 1

  // Initialize map to be uncovered
  vector<vector<int>> covered_map; // accessed by [x][y]
  covered_map.resize(max_width + 1);
  for (int x = 0; x <= max_width; ++x) {
    covered_map[x].resize(max_height + 1);
    for (int y = 0; y <= max_height; ++y) {
      covered_map[x][y] = 0;
    }
  }


  // Cover the map with the line segments, accounting differently to track Part 1 and Part 2 answers separately
  int hor_vert_intersections = 0;
  for (auto l:line_segments) {
    if (IsHorizontal(l)) { // consistent y
      int y = l.first.second;

      // Line segments can have any direction; orient before marking to avoid confusion and bugs
      int x_start, x_end;
      if (l.first.first <= l.second.first) { // line segment can be a single point
        x_start = l.first.first;
        x_end = l.second.first;
      }
      else {
        x_start = l.second.first;
        x_end = l.first.first;
      }

      for (int x = x_start; x <= x_end; ++x) {
        int curr = covered_map[x][y];
        if (curr == 1) {
          ++hor_vert_intersections;
        }

        covered_map[x][y] = curr + 1;
      }
    }
    else if (IsVertical(l)) { // consistent x
      int x = l.first.first;

      // Line segments can have any direction; orient before marking to avoid confusion and bugs
      int y_start, y_end;
      if (l.first.second <= l.second.second) { // line segment can be a single point
        y_start = l.first.second;
        y_end = l.second.second;
      }
      else {
        y_start = l.second.second;
        y_end = l.first.second;
      }

      for (int y = y_start; y <= y_end; ++y) {
        int curr = covered_map[x][y];
        if (curr == 1) {
          ++hor_vert_intersections;
        }

        covered_map[x][y] = curr + 1;
      }
    }
  }

  cout << "Part 1 answer: " << hor_vert_intersections << endl;
  
  // Part 2: now add in diagonal lines
  int all_intersections = hor_vert_intersections;
  for (auto l:line_segments) {
    if (!IsHorizontal(l) && !IsVertical(l)) { // diagonal
      // Check assumption that line is 45 degrees
      if (abs(l.first.first - l.second.first) != abs(l.first.second - l.second.second)) {
        cout << "Line is not diagonal: starting at " << l.first.first << "," << l.first.second << " and ending at " << l.second.first << "," << l.second.second << endl;
        return -1;
      }

      // Line segments can have any direction; orient with x values before marking to avoid confusion and bugs
      int x_start, x_end, y_start, y_end;
      if (l.first.first <= l.second.first) { // line segment can be a single point
        x_start = l.first.first;
        y_start = l.first.second;

        x_end = l.second.first;
        y_end = l.second.second;
      }
      else {
        x_start = l.second.first;
        y_start = l.second.second;

        x_end = l.first.first;
        y_end = l.first.second;
      }

      for (int x = x_start, y = y_start; x <= x_end; ++x) {
        int curr = covered_map[x][y];
        if (curr == 1) {
          ++all_intersections;
        }

        covered_map[x][y] = curr + 1;

        // Line segments can have any direction; check before moving y to next point (loop handles x)
        if (y < y_end) {
          ++y;
        }
        else {
          --y;
        }
      }
    }
  }

  cout << "Part 2 answer: " << all_intersections << endl;

  return 0;
}
