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

// ParseSeparatedInts takes in a string and the separator used:
//   The first return value is the parsed list of ints
//   The second return value represents the success of the operation; negative if failure
// Runtime complexity: linear in the length of the string
// This is in fileutil.hpp because it is a common format for input files
std::pair<std::vector<int>, int> ParseSeparatedInts(const std::string str, const std::string sep);

#endif // FILE_UTIL_H