// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 17 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the highest y position it reaches on this trajectory?
// Part 2: How many distinct initial velocity values cause the probe to be within the target area after any step?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <regex>
#include <cmath>

using namespace std;

// SumNaturalNumbersUpTo returns the sum of natural numbers (ex. 1+2+...+n) up to n
int SumNaturalNumbersUpTo(int n) {
  if (n == 1) { return n; }
  else { return n + SumNaturalNumbersUpTo(n-1); }
}

bool DoesInitialVelocityPassWithinTarget(
  const pair<pair<int,int>, pair<int,int>>& target_area,
  const int& initial_vel_x,
  const int& initial_vel_y)
{
  int vel_x = initial_vel_x, vel_y = initial_vel_y;
  int pos_x = 0, pos_y = 0;
  // int step = 1;

  while (pos_x < target_area.first.second && pos_y > target_area.second.first) {
    // Perform movement
    pos_x += vel_x;
    pos_y += vel_y;
    if (vel_x > 0) { --vel_x; }
    else if (vel_x < 0) { ++vel_x; }
    --vel_y;

    // Check if position is within target area
    if (pos_x >= target_area.first.first && pos_x <= target_area.first.second && pos_y >= target_area.second.first && pos_y <= target_area.second.second) { return true; }

    // ++step;
  }

  return false;
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day17/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  regex r("target area: x=([-]{0,1}\\d+)..([-]{0,1}\\d+), y=([-]{0,1}\\d+)..([-]{0,1}\\d+)");
  smatch m;
  regex_search(file_results.first[0], m, r);
  if (m.size() != 5) {
    cout << "Unexpected file format" << endl;
    return -1;
  }

  // ordered by (min_x, max_x), (min_y, max_y)
  pair<pair<int,int>, pair<int,int>> target_area(pair<int,int>(stoi(m[1]), stoi(m[2])), pair<int,int>(stoi(m[3]), stoi(m[4])));

  // Probe starts at (x,y) = (0,0)

  // Part 1: Find the initial velocity that causes the probe to reach the highest y position and still eventually be within the target area after any step.
  // we only have to worry about the y coordinate for now, since the x position and velocity do not affect the y position and velocity (following physics principles)
  // Whether the y targets are positive or negative (if one is positive and the other is negative, the answer is infinite b/c y will eventually cross through the point 0), the goal position will be the sum of consecutive natural numbers, and we want to maximize the smallest of the natural numbers (that, minus 1, will be the initial y velocity)
  // The upper bound on the smallest of the natural numbers will be abs(min_y), since a step would go from (0,0) to (x,min_y) before and (x,min_y+1) after gravity had turned it around
  int answer;
  if (target_area.second.first < 0) {
    answer = SumNaturalNumbersUpTo(abs(target_area.second.first) - 1);
  }
  else {
    answer = SumNaturalNumbersUpTo(target_area.second.first);
  }

  cout << "Part 1 answer: " << answer << endl;

  // Part 2:
  // We know our input is positive x values, and negative y values
  // The only valid x velocities are between n (where the minimized sum of natural numbers up to n >= min_x), and max_x (then it will shoot past the target area)
  int min_vel_x = 0;
  for (int x = 1; x <= target_area.first.first; ++x) {
    int sum_n = SumNaturalNumbersUpTo(x);
    if (sum_n >= target_area.first.first) {
      min_vel_x = x;
      break;
    }
  }

  vector<pair<int,int>> possible_velocities;
  for (int x = min_vel_x; x <= target_area.first.second; ++x) {
    // The only valid y velocities are between min_y (largest negative number to reach target area), and the n for part 1 answer (a high positive number that shoots just within the target)
    for (int y = target_area.second.first; y <= abs(target_area.second.first) - 1; ++y) {
      if (DoesInitialVelocityPassWithinTarget(target_area, x, y)) {
        possible_velocities.push_back(pair<int,int>(x,y));
      }
    }
  }

  cout << "Part 2 answer: " << possible_velocities.size() << endl;

  return 0;
}
