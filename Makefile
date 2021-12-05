# Author: Laura Galbraith
# Description: Compiler instructions for programs solving Advent of Code 2021

COMPILER = g++
FILE_UTIL_CPP = util/fileutil.cpp
DEBUG_FLAGS = -static-liblsan -fsanitize=leak

help:
	@echo "Try targets 'all' or 'day1_puzz1_debug.out'"
	@echo "If you get a message like 'make: *** No rule to make target 'day25.out'.  Stop.' : check that the necessary depencencies for day25 exist"

all: day1_puzz1.out day1_puzz2.out day2.out day3_puzz1.out day3_puzz2.out day4.out

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
	rm -f *.out
