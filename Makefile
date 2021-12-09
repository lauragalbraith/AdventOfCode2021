# Author: Laura Galbraith
# Description: Compiler instructions for programs solving Advent of Code 2021

COMPILER = g++
FILE_UTIL_CPP = util/fileutil.cpp
DEBUG_FLAGS = -static-liblsan -fsanitize=leak

help:
	@echo "Try targets 'all' or 'day01_puzz1_debug.out'"
	@echo "If you get a message like 'make: *** No rule to make target 'day25.out'.  Stop.' : check that the necessary depencencies for day25 exist"

all: day01_puzz1.out day01_puzz2.out day02.out day03_puzz1.out day03_puzz2.out day04.out day05.out day06.out day07.out day08.out day09.out day10.out day11.out day12.out day13.out day14.out day15.out day16.out day17.out day18.out day19.out day20.out day21.out day22.out day23.out day24.out day25.out

day%_puzz1_debug.out: $(FILE_UTIL_CPP) day%/puzzle1/main.cpp
	$(COMPILER) $(DEBUG_FLAGS) $^ -o $@

day%_puzz2_debug.out: $(FILE_UTIL_CPP) day%/puzzle2/main.cpp
	$(COMPILER) $(DEBUG_FLAGS) $^ -o $@

day%_debug.out: $(FILE_UTIL_CPP) day%/main.cpp
	$(COMPILER) $(DEBUG_FLAGS) $^ -o $@

day%_puzz1.out: $(FILE_UTIL_CPP) day%/puzzle1/main.cpp
	$(COMPILER) $^ -o $@

day%_puzz2.out: $(FILE_UTIL_CPP) day%/puzzle2/main.cpp
	$(COMPILER) $^ -o $@

day%.out: $(FILE_UTIL_CPP) day%/main.cpp
	$(COMPILER) $^ -o $@

clean:
	rm -f *.out test*.txt
