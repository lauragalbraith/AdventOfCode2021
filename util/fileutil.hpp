// fileutil.hpp: Laura Galbraith
// Description: header file for useful file-related functions for The Advent Of Code 2021

#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <tuple>
#include <vector>
#include <string>

// ReadLinesFromFile takes in the name of a file and returns two values:
//   The first return value represents the file contents as a vector of strings
//   The second return value represents the success of the operation; negative if failure
// Runtime complexity: linear in the size of the file
std::pair<std::vector<std::string>, int> ReadLinesFromFile(const std::string file_name);

#endif // FILE_UTIL_H