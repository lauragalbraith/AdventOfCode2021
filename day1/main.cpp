// main.cpp: Laura Galbraith
// Description: A day's program for Advent Of Code
// See: https://adventofcode.com/2021

#include <iostream>

int main()
{
  // an example from https://developers.google.com/edu/c++/getting-started
  for (int i=0; i < 6; i++) {
    for (int j=0; j < 4; j++) {
      std::cout.width(17);
      std::cout << std::left << "Hello World!";
    }
    std::cout << std::endl;
  }
  return 0;
}
