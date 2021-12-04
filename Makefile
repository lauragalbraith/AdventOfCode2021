# Author: Laura Galbraith
# Description: Compiler instructions for programs solving Advent of Code 2021

COMPILER = g++
FILE_UTIL_CPP = util/fileutil.cpp
DEBUG_FLAGS = -static-liblsan -fsanitize=leak

# TODO: using https://makefiletutorial.com/ , I'm trying to understand how I could define one or two rules to compile any individual day. Maybe this is not what Makefile is made for, because I'm not sure I can use any of the "Automatic variables" to specify a variable path on which to drop the executable. But maybe that wouldn't make sense, since I am running 'make' from the top directory; the output should end up here; perhaps "day1.out"?

all: day1_puzz1.out day1_puzz2.out day2 day3

day1_puzz1.out: day1/input.txt day1/puzzle1/main.cpp
	$(COMPILER) day1/puzzle1/main.cpp -o $@

day1_puzz2.out: day1/input.txt day1/puzzle2/main.cpp
	$(COMPILER) day1/puzzle2/main.cpp -o $@

day2:

day3:

%_puzz1.out: %/input.txt %_puzz1_wo_txt
	echo "Must call other make command"

%_puzz1_wo_txt: %/puzzle1/main.cpp # TODO %/puzzle1/input.txt want to include as a dependency, but not pass it to g++ - perhaps make a hidden target which is actually this but makes this target a dependency?
	$(COMPILER) $< -o $@

# %.o : %.c
      #  $(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	# TODO