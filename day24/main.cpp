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
#include <regex>
#include <stdexcept>

using namespace std;

// TODO FINALLY remove unused code (consider: instruction commands I've rendered obselete)
// TODO FINALLY remove debugging statements
// TODO FINALLY evaluate which imports are needed

// Define ALU operations
typedef void (*instruction_cmd)(long long int& a, const long long int b);

void Input(long long int& a, const long long int b) { a = b; }

void Add(long long int& a, const long long int b) { a = a + b; }

void Multiply(long long int& a, const long long int b) { a = a * b; }

void Divide(long long int& a, const long long int b) {
  if (b == 0) {
    throw invalid_argument("cannot divide by 0");
  }

  // Check which way to round the value; should be "toward zero"
  if ((a < 0 && b < 0) || (a > 0 && b > 0)) {
    // result will be positive
    a = (long long int)floor(1.0 * a / b);
  }
  else {
    // result will be negative
    a = (long long int)ceil(1.0 * a / b);
  }
}

void Modulo(long long int& a, const long long int b) { a = a % b; }

void NotEquals(long long int& a, const long long int b) { a = a != b ? 1 : 0; }

// Define types/enums to make it easier to determine which variable(s) are being manipulated
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

class Instruction {
  private:
    ALUVariable arg2;
    bool arg2_literal_defined;
    long long int arg2_literal;

  public:
    string command;
    ALUVariable arg1;

    Instruction(const string& line) {
      smatch m;
      regex_search(line, m, regex("^([a-z]{3}) ([a-z])( ([a-z0-9-]+)){0,1}"));
      if (m.size() != 5) {
        throw invalid_argument("unexpected line format");
      }

      // parse command from instruction
      this->command = m[1];

      // parse first argument to the instruction
      this->arg1 = FromInstructionChar(m.str(2)[0]);

      // parse second argument to the instruction
      this->arg2 = VARIABLE_COUNT;
      this->arg2_literal_defined = false;

      if (m[4] != "") {
        this->arg2 = FromInstructionChar(m.str(4)[0]);
        if (this->arg2 == VARIABLE_COUNT) {
          this->arg2_literal_defined = true;
          this->arg2_literal = stoi(m[4]);
        }
      }
    }

    // Assignment operator
    Instruction& operator=(const Instruction& other) {
      if (this != &other) {
        this->command = other.command;
        this->arg1 = other.arg1;
        this->arg2 = other.arg2;
        this->arg2_literal_defined = other.arg2_literal_defined;
        this->arg2_literal = other.arg2_literal;
      }

      return *this;
    }

    ALUVariable GetSecondArgumentVariable() const {
      return this->arg2;
    }

    pair<long long int,bool> GetSecondArgumentLiteral() const {
      return pair<long long int,bool>(this->arg2_literal, this->arg2_literal_defined);
    }
};

// GetNextInstructionLine returns the next instruction line after curr, skipping any lines that start with "#"
int GetNextInstructionLine(const vector<string>& lines, const int curr) {
  int next = curr + 1;
  while (next < lines.size() && lines[next][0] == '#') {
    ++next;
  }

  return next;
}

constexpr long long int possible_input_digits[9] = {1,2,3,4,5,6,7,8,9};
const long long int max_digit = possible_input_digits[8];

class ALUState {
  private:
    vector<long long int> variables;

    void copy(const ALUState& other) {
      this->variables.resize(other.variables.size());
      for (int i = 0; i < other.variables.size(); ++i) {
        this->variables[i] = other.variables[i];
      }
    }

  public:
    // ALU state starts with all variables at value 0
    ALUState() {
      for (int i = 0; i < VARIABLE_COUNT; ++i) {
        this->variables.push_back(0);
      }
    }

    // copy constructor
    ALUState(const ALUState& other) {
      this->copy(other);
    }

    // assignment operator
    ALUState& operator=(const ALUState& other) {
      if (this != &other) {
        this->copy(other);
      }
      return *this;
    }

    // operator< is ecessary to implement for class to be used as map key
    bool operator<(const ALUState& other) const {
      // unexpected corner case; we always expect ALU states to have the same number of variables at all times
      if (this->variables.size() != other.variables.size()) {
        return this->variables.size() < other.variables.size();
      }

      for (int i = 0; i < this->variables.size(); ++i) {
        if (this->variables[i] < other.variables[i]) {
          return true;
        }
        else if (this->variables[i] > other.variables[i]) {
          return false;
        }
        // if they are equal, continue
      }

      return false;
    }

    // Returns the value of this ALU state's specified variable
    const long long int GetVariableValue(const ALUVariable var) const {
      if (var >= VARIABLE_COUNT) {
        throw range_error("ALU variable index");
      }
      else if (var >= this->variables.size()) {
        throw logic_error("ALU state has unexpected number of variables");
      }

      return this->variables[var];
    }

    // Perform instruction on a single variable
    // Returns a list of possible ALU states reached by taking input into this ALU state's variable (indicated by a_var)
    vector<ALUState> PerformInstruction(instruction_cmd instrct, const ALUVariable a_var) {
      // Since the only single-input instruction is Input, implement that branching logic
      vector<ALUState> all_possible_states;
      for (int i = 0; i < sizeof(possible_input_digits)/sizeof(possible_input_digits[0]); ++i) {
        ALUState state(*this);

        (*instrct)(state.variables[a_var], possible_input_digits[i]);

        all_possible_states.push_back(state);
      }

      // cout << "returning " << all_possible_states.size() << " states from single-var instruction" << endl;
      return all_possible_states;
    }

    // Perform instruction on two variables
    vector<ALUState> PerformInstruction(instruction_cmd instrct, const ALUVariable a_var, const ALUVariable b_var) {
      (*instrct)(this->variables[a_var], this->variables[b_var]);

      return vector<ALUState>(1, *this);
    }

    // Perform instruction on one variables and a literal
    vector<ALUState> PerformInstruction(instruction_cmd instrct, const ALUVariable a_var, const long long int b_literal) {
      (*instrct)(this->variables[a_var], b_literal);

      return vector<ALUState>(1, *this);
    }

    ~ALUState() {}
};

// defining the << operator function to ease printing
ostream& operator<<(ostream& os, const ALUState& c) {
  for (int i = 0; i < VARIABLE_COUNT; ++i) {
    os << c.GetVariableValue(static_cast<ALUVariable>(i)); // TODO also print the name of the variable
    if (i < VARIABLE_COUNT - 1) {
      os << ",";
    }
  }
  return os;
}

// ConvertDigitListToNumber returns the represented number
// ex. if passed {1,2,3}, it will return 123
// Returns 0 if the list is empty
unsigned long long int ConvertDigitListToNumber(const vector<unsigned int>& list) {
  unsigned long long int result = 0;
  for (auto digit:list) {
    result = result*10 + digit;
  }

  return result;
}

// Values determined by studying input.txt
const long long int z_multiplier = 26;
const string z_divider_instruction = "div z " + to_string(z_multiplier);
const long long int max_addition = 15;

// If a state's z value gets bigger than can be divided down to zero, we know it will not be valid
// by studying input.txt, we notice that z is always of the form  26*(26*...26*(digitX + literal-from-file) + digitZ + literal-from-file)... ) ... + digitY + literal-from-file
// where MAX(literal-from-file) is 15
long long int ComputeMaximumMultipliedZValue(const long long int z_dividers_remaining) {
  if (z_dividers_remaining == 0) {
    return 0;
  }
  else if (z_dividers_remaining == 1) {
    return max_digit + max_addition;
  }
  else {
    return z_multiplier * ComputeMaximumMultipliedZValue(z_dividers_remaining-1) + max_digit + max_addition;
  }
}

int main() {
  // The original input.txt was studied and modified to reduce instructions to speed up code; see explanatory comments in the file
  pair<vector<string>, int> file_results = ReadLinesFromFile("day24/input-modified.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Part 1:

  // Pre-process instructions to more efficiently determine acceptable model numbers
  vector<string> instructions;
  long long int z_dividers_remaining = 0;
  for (auto line:file_results.first) {
    // Count the number of times the defining z-divider instruction appears
    if (line == z_divider_instruction) {
      ++z_dividers_remaining;
    }
  }

  long long int max_possibly_valid_z_value = ComputeMaximumMultipliedZValue(z_dividers_remaining);
  // cout << "Max possible z value starts as " << max_possibly_valid_z_value << " with " << z_dividers_remaining << " divider instructions remaining" << endl;

  // Create initial possible ALU state
  ALUState initial = ALUState();
  map<ALUState, vector<unsigned int>> maxed_possible_states; // value of this map is the greatest possible series of inputs to get to the state
  maxed_possible_states[initial] = vector<unsigned int>();

  // Process each instruction on all possible ALU states, given input possibilities
  for (int l = GetNextInstructionLine(file_results.first, -1); l < file_results.first.size(); l = GetNextInstructionLine(file_results.first, l)) {
    // cout << "Performing instruction " << l << ": " << file_results.first[l] << endl;
    Instruction i(file_results.first[l]);

    // Parse file input: parse command from instruction
    instruction_cmd cmd_func;
    if (i.command == "inp") {
      cmd_func = &Input;
    }
    else if (i.command == "add") {
      cmd_func = &Add;
    }
    else if (i.command == "mul") {
      if (i.GetSecondArgumentLiteral().second && i.GetSecondArgumentLiteral().first == 1) {
        continue; // skip to next instruction, because multiplying by 1 is a no-op
      }

      if (i.GetSecondArgumentLiteral().second && i.GetSecondArgumentLiteral().first == 0) {
        // studying input.txt reveals that when a variable is multiplied by 0, the next instruction is always adding a value to that variable, effectively setting that variable equal to what's specified in the second instruction
        // Combine those two instructions into a single command: input b into a
        l = GetNextInstructionLine(file_results.first, l);
        // cout << "(combining with instruction " << file_results.first[l] << ")" << endl;
        cmd_func = &Input;
        Instruction equivalent(file_results.first[l]);
        i = equivalent;
      }
      else {
        cmd_func = &Multiply;
      }
    }
    else if (i.command == "div") {
      if (i.GetSecondArgumentLiteral().second && i.GetSecondArgumentLiteral().first == 1) {
        continue; // skip to next instruction, because dividing by 1 is a no-op
      }

      // update continuous z-divider instruction count as we go
      if (file_results.first[l] == z_divider_instruction) {
        --z_dividers_remaining;
        max_possibly_valid_z_value = ComputeMaximumMultipliedZValue(z_dividers_remaining);
        // cout << "Max possible z value is now " << max_possibly_valid_z_value << " with " << z_dividers_remaining << " divider instructions remaining" << endl;
      }

      cmd_func = &Divide;
    }
    else if (i.command == "mod") {
      cmd_func = &Modulo;
    }
    else if (i.command == "eql") {
      // studying input.txt reveals that all eql instructions come in pairs that equate to a not-equal operation
      // so we'll skip the next instruction, and now run a not-equals operation
      l = GetNextInstructionLine(file_results.first, l);
      // cout << "(combining with instruction " << file_results.first[l] << ")" << endl;
      cmd_func = &NotEquals;
    }
    else {
      cout << "Unexpected instruction: " << i.command << endl;
      return -1;
    }

    map<ALUState, vector<unsigned int>> next_maxed_possible_states;
    for (auto it = maxed_possible_states.begin(); it != maxed_possible_states.end(); ++it) {
      ALUState current_state = it->first;
      vector<unsigned int> current_input_list = it->second;
      // cout << "Handling current ALU state: " << current_state << endl;

      vector<ALUState> next_states;
      if (i.GetSecondArgumentVariable() != VARIABLE_COUNT) {
        // cout << "Processing double-var instruction type..." << endl;
        next_states = current_state.PerformInstruction(cmd_func, i.arg1, i.GetSecondArgumentVariable());
      }
      else if (i.GetSecondArgumentLiteral().second) {
        // cout << "Processing one-var one-val instruction type..." << endl;
        next_states = current_state.PerformInstruction(cmd_func, i.arg1, i.GetSecondArgumentLiteral().first);
      }
      else {
        // cout << "Processing single-var instruction type..." << endl;
        next_states = current_state.PerformInstruction(cmd_func, i.arg1);
      }

      for (auto state:next_states) {
        // Determine if this next state has a chance of representing a valid model number before adding it to the tracking map
        if (state.GetVariableValue(z) > max_possibly_valid_z_value) {
          // cout << "Rejecting state with z value " << state.GetVariableValue(z) << " as it is too high for the remaining " << z_dividers_remaining << " dividing instructions to reduce" << endl;
          continue;
        }

        vector<unsigned int> next_input_list(current_input_list);
        if (i.command == "inp") {
          next_input_list.push_back(state.GetVariableValue(i.arg1));
        }

        // cout << "Next state is: " << state << " with input list:";
        // for (auto input:next_input_list) {
        //   cout << input << ",";
        // }
        // cout << endl;

        if (next_maxed_possible_states.find(state) != next_maxed_possible_states.end()) {
          if (l >= 98) {
            // cout << "Evaluating " << state << " already stored in map with input list " << ConvertDigitListToNumber(next_maxed_possible_states[state]) << " against new input list " << ConvertDigitListToNumber(next_input_list) << endl;
          }

          // If this next state has already been reached by an already-processed ALU state, then store the input list that represents the greater number
          if (ConvertDigitListToNumber(next_maxed_possible_states.at(state)) > ConvertDigitListToNumber(next_input_list)) {
            // cout << "Using the stored input list" << endl;
            next_input_list = next_maxed_possible_states.at(state);
          }
        }
        else {
          if (l >= 98) {
            // cout << "Inserting " << state << " into the map for the first time with input list " << ConvertDigitListToNumber(next_input_list) << endl;
          }
        }

        next_maxed_possible_states[state] = next_input_list;
      }
    }

    // Move to next possible states for next instruction
    if (next_maxed_possible_states.size() != maxed_possible_states.size()) {
      // cout << "Now handling " << next_maxed_possible_states.size() << " states" << endl;
    }

    maxed_possible_states = next_maxed_possible_states;
  }

  // Pen and paper suggested answer 99994469899267, but inevitably missed something, because it's too high
  // TODO IDEAS: rather than compute everything, form strings for what the registers equal, and print out z at the end (can simplify certain instructions this way; mul x 0 means the whole expression gets wiped out and goes to 0); everything else gets prepended with (, appended with ), and then an operator written out, potentially also with () around it
  // TODO idea go thru instructions in reverse order; z must be 0, so "add z y" means z and y are opposite but equal positive/negative numbers; but I don't know how to represent all possibilities for that...
  // TODO idea: try all 1s, all 2s, all 3s, etc. as input and what that gives us; or 12345.... 98765....
  // TODO idea: if deleting is slow, keep a list of things that need to be deleted, and delete them later! Or figure out another way to only use 1 map the whole time (could include instruction number in ALU state, OR remove element from map as we use it - might have to use a while loop/repeatedly call begin() to get all elements to be handled correctly) - **test how iterating works while you delete; if I can guarantee order, I could insert higher ALU instructions before the current iterator, or just write skip code if state is beyond current instruction

  // Determine which valid output has the greatest model number
  unsigned long long int max_model_number = 0;
  for (auto state:maxed_possible_states) {
    if (state.first.GetVariableValue(z) == 0) {
      unsigned long long int valid_model_number = ConvertDigitListToNumber(state.second);
      if (valid_model_number > max_model_number) {
        max_model_number = valid_model_number;
      }
    }
  }

  cout << "Part 1 answer: " << max_model_number << endl;

  // Part 2:
  // TODO

  return 0;
}
