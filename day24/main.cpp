// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 24 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the largest model number accepted by MONAD?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <functional>
#include <regex>

using namespace std;

void Add(int& a, const int b) { a = a + b; }

void Multiply(int& a, const int b) { a = a * b; }

void Divide(int& a, const int b) {
  // Check which way to round the value; should be "toward zero"
  if ((a < 0 && b < 0) || (a > 0 && b > 0)) {
    // result will be positive
    a = (int)floor(1.0 * a / b);
  }
  else {
    // result will be negative
    a = (int)ceil(1.0 * a / b);
  }
}

void Modulo(int& a, const int b) { a = a % b; }

void Equals(int& a, const int b) { a = a == b ? 1 : 0; }

enum ALUVariable { w=0, x=1, y=2, z=3, VARIABLE_COUNT=4 };

// returns VARIABLE_COUNT if char does not match a variable
ALUVariable FromInstructionChar(const char& c) {
  switch(c) {
    case 'w': return w;
    case 'x': return x;
    case 'y': return y;
    case 'z': return z;

    default: return VARIABLE_COUNT;
  }
}

class ALUState {
  private:
    vector<int> variables;
    unsigned int next_instruction;

    static constexpr int possible_input_digits[9] = {1,2,3,4,5,6,7,8,9};

    void copy(const ALUState& other) {
      this->variables.resize(other.variables.size());
      for (int i = 0; i < other.variables.size(); ++i) {
        this->variables[i] = other.variables[i];
      }
      this->next_instruction = other.next_instruction;
    }

  public:
    ALUState() {
      for (int i = 0; i < VARIABLE_COUNT; ++i) {
        this->variables.push_back(0);
      }
      this->next_instruction = 0;
    }

    ALUState(const ALUState& other) {
      this->copy(other);
    }

    ALUState& operator=(const ALUState& other) {
      if (this != &other) {
        this->copy(other);
      }
      return *this;
    }

    unsigned int GetAndIncrementNextInstruction() {
      return this->next_instruction++;
    }

    bool CompleteAndValid(const int total_instructions) {
      return this->next_instruction == total_instructions && this->variables[z] == 0;
    }

    vector<ALUState> Input(const unsigned int var_idx) { // TODO HERE what I'm imaginging is tracking ALUState as keys in a map, where value is a vector of inputs that have produced said alustate; when we go to insert a new alustate in the map that would map to an existing key, compare the values and keep the one in the map that represents the largest model number; on each instruction iteration, take from the old map and insert into a brand-new map (to avoid having to track which ALU state is on which step)
      vector<ALUState> all_possible_states;
      for (int i = 0; i < sizeof(ALUState::possible_input_digits); ++i) {
        ALUState state(*this);

        state.variables[var_idx] = ALUState::possible_input_digits[i];
        all_possible_states.push_back(state);
      }

      return all_possible_states;
    }

    ~ALUState() {}
};

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day24/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // TODO idea go thru instructions in reverse order; z must be 0, so "add z y" means z and y are opposite but equal positive/negative numbers; but I don't know how to represent all possibilities for that...
  // TODO idea: try all 1s, all 2s, all 3s, etc. as input and what that gives us; or 12345.... 98765....

  // Part 1:
  // Use queue to read inputs for instructions  - TODO HERE updating file for new scheme
  queue<int> digit_queue;

  // Parse file input
  vector<function<void()>> execution_list;

  regex instruction_rgx("^([a-z]{3}) ([a-z])( ([a-z0-9-]+)){0,1}");
  for (auto line:file_results.first) {
    smatch m;
    regex_search(line, m, instruction_rgx);
    if (m.size() != 5) {
      cout << "unexpected line format: " << line << endl;
      return -1;
    }

    int* a_ptr = NULL;
    int* b_ptr = NULL;
    int literal_val;

    // parse first argument to the instruction
    switch(m.str(2)[0]) {
      case 'w': a_ptr = w; break;
      case 'x': a_ptr = x; break;
      case 'y': a_ptr = y; break;
      case 'z': a_ptr = z; break;

      default: {
        cout << "invalid first argument: " << m[2] << endl;
        return -1;
      }
    }

    // parse second argument to the instruction
    if (m[4] != "") {
      switch(m.str(4)[0]) {
        case 'w': b_ptr = w; break;
        case 'x': b_ptr = x; break;
        case 'y': b_ptr = y; break;
        case 'z': b_ptr = z; break;

        default: {
          literal_val = stoi(m[4]);
          break;
        }
      }
    }

    // parse instruction
    function<void()> instruction;
    if (m[1] == "inp") {
      instruction = [a_ptr, &digit_queue]() {
        *a_ptr = digit_queue.front();
        digit_queue.pop();
      };
    }
    else if (m[1] == "add") {
      if (b_ptr != NULL) {
        instruction = [a_ptr, b_ptr]() { Add(*a_ptr, *b_ptr); };
      }
      else {
        instruction = [a_ptr, literal_val]() { Add(*a_ptr, literal_val); };
      }
    }
    else if (m[1] == "mul") {
      if (b_ptr != NULL) {
        instruction = [a_ptr, b_ptr]() { Multiply(*a_ptr, *b_ptr); };
      }
      else {
        instruction = [a_ptr, literal_val]() { Multiply(*a_ptr, literal_val); };
      }
    }
    else if (m[1] == "div") {
      if (b_ptr != NULL) {
        instruction = [a_ptr, b_ptr]() { Divide(*a_ptr, *b_ptr); };
      }
      else {
        instruction = [a_ptr, literal_val]() { Divide(*a_ptr, literal_val); };
      }
    }
    else if (m[1] == "mod") {
      if (b_ptr != NULL) {
        instruction = [a_ptr, b_ptr]() { Modulo(*a_ptr, *b_ptr); };
      }
      else {
        instruction = [a_ptr, literal_val]() { Modulo(*a_ptr, literal_val); };
      }
    }
    else if (m[1] == "eql") {
      if (b_ptr != NULL) {
        instruction = [a_ptr, b_ptr]() { Equals(*a_ptr, *b_ptr); };
      }
      else {
        instruction = [a_ptr, literal_val]() { Equals(*a_ptr, literal_val); };
      }
    }
    else {
      cout << "Unexpected instruction: " << m[1] << endl;
      return -1;
    }

    execution_list.push_back(instruction);
  }

  // Since we want to determine the largest valid model number, start with the largest 14-digit number and decrease from there
  unsigned long long int model_number;
  bool valid = false;
  // Pen and paper suggested answer 99994469899267, but inevitably missed something, because it's too high
  for (model_number = 99999999999999; !valid && model_number > 0; --model_number) {
    // TODO HERE IDEAS: rather than compute everything, form strings for what the registers equal, and print out z at the end (can simplify certain instructions this way; mul x 0 means the whole expression gets wiped out and goes to 0); everything else gets prepended with (, appended with ), and then an operator written out, potentially also with () around it
    // TODO other idea: track multiple possible states like one reddit user did, combining states that are the same when possible

    // Prepare digits to be input
    pair<queue<int>,bool> digit_queue_result = MostToLeastSignificantNonZeroDigits(model_number);
    if (!digit_queue_result.second) {
      continue;
    }

    digit_queue = digit_queue_result.first;

    // cout << "Model number is " << model_number << endl;

    // Start all values at 0 when the instructions start
    *w = 0;
    *x = 0;
    *y = 0;
    *z = 0;

    // Execute the instructions, in order
    for (auto f:execution_list) {
      f(); // TODO idea: could remove all the "div ? 1" instructions from the input, since they do nothing
      // cout << "variables are now w:" << *w << ", x:" << *x << ", y:" << *y << ", z:" << *z << endl;
    }

    if (*z == 0) {
      valid = true;
    }

    // cout << endl; // TODO FINALLY remove debugging statements
  }

  cout << "Part 1 answer: " << model_number << endl;

  // Clean up memory
  delete w;
  delete x;
  delete y;
  delete z;

  // Part 2:
  // TODO

  return 0;
}
