# Author: Laura Galbraith
# Description: Compiler instructions for programs solving Advent of Code 2021

COMPILER = g++
FILE_UTIL_CPP = util/fileutil.cpp
DEBUG_FLAGS = -static-liblsan -fsanitize=leak

help:
	@echo "Try targets 'all' or 'day1_puzz1_debug.out'"
	@echo "If you get a message like 'make: *** No rule to make target 'day25_both.out'.  Stop.' : check that the necessary depencencies for day25 exist"

all: day1_puzz1.out day1_puzz2.out day2_both.out day3_puzz1.out day3_puzz2_dep.out

day%_puzz1_debug.out: day%/puzzle1/main.cpp
	$(COMPILER) $(DEBUG_FLAGS) $^ -o $@

day%_puzz2_debug.out: day%/puzzle2/main.cpp
	$(COMPILER) $(DEBUG_FLAGS) $^ -o $@

day%_both_debug.out: day%/main.cpp
	$(COMPILER) $(DEBUG_FLAGS) $^ -o $@

day%_puzz1.out: day%/puzzle1/main.cpp
	$(COMPILER) $^ -o $@

day%_puzz2.out: day%/puzzle2/main.cpp
	$(COMPILER) $^ -o $@

day%_both.out: day%/main.cpp
	$(COMPILER) $^ -o $@

# TODO Until more files have been converted to use fileutil.cpp, separate rules are needed for independent/dependent cases
day3_puzz2_dep.out: $(FILE_UTIL_CPP) day3/puzzle2/main.cpp
	$(COMPILER) $^ -o $@

day4_both_debug.out: $(FILE_UTIL_CPP) day4/main.cpp
	$(COMPILER) $(DEBUG_FLAGS) $^ -o $@

clean:
	rm -f *.out
