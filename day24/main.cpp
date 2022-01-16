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
// TODO FINALLY evaluate which imports are needed
#include <regex>
#include <stdexcept>

using namespace std;

// TODO FINALLY remove unused code

// Define ALU operations
typedef void (*alu_instruction)(long long int& a, const long long int b);

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

void Equals(long long int& a, const long long int b) { a = a == b ? 1 : 0; }

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

constexpr long long int possible_input_digits[9] = {1,2,3,4,5,6,7,8,9};

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
    vector<ALUState> PerformInstruction(alu_instruction instrct, const ALUVariable a_var) {
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
    vector<ALUState> PerformInstruction(alu_instruction instrct, const ALUVariable a_var, const ALUVariable b_var) {
      (*instrct)(this->variables[a_var], this->variables[b_var]);

      return vector<ALUState>(1, *this);
    }

    // Perform instruction on one variables and a literal
    vector<ALUState> PerformInstruction(alu_instruction instrct, const ALUVariable a_var, const long long int b_literal) {
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

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day24/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Part 1:

  // Create initial possible ALU state
  ALUState initial = ALUState();
  map<ALUState, vector<unsigned int>>* maxed_possible_states = new map<ALUState, vector<unsigned int>>(); // value of this map is the greatest possible series of inputs to get to the state
  (*maxed_possible_states)[initial] = vector<unsigned int>();

  // Process each instruction on all possible ALU states, given input possibilities
  regex instruction_rgx("^([a-z]{3}) ([a-z])( ([a-z0-9-]+)){0,1}");
  for (int l = 0; l < file_results.first.size(); ++l) {
    cout << "Performing instruction " << l << ": " << file_results.first[l] << endl; 
    smatch m;
    regex_search(file_results.first[l], m, instruction_rgx);
    if (m.size() != 5) {
      cout << "unexpected line format: " << file_results.first[l] << endl;
      return -1;
    }

    // Parse file input: parse first argument to the instruction
    ALUVariable arg1 = FromInstructionChar(m.str(2)[0]);

    // Parse file input: parse second argument to the instruction
    ALUVariable arg2 = VARIABLE_COUNT;
    bool arg2_literal_defined = false;
    int arg2_literal;

    if (m[4] != "") {
      arg2 = FromInstructionChar(m.str(4)[0]);
      if (arg2 == VARIABLE_COUNT) {
        arg2_literal_defined = true;
        arg2_literal = stoi(m[4]);
      }
    }

    // Parse file input: parse instruction
    alu_instruction instruction;
    // cout << "Instruction type: " << m[1] << endl;
    if (m[1] == "inp") {
      instruction = &Input;
    }
    else if (m[1] == "add") {
      instruction = &Add;
    }
    else if (m[1] == "mul") {
      if (arg2_literal_defined && arg2_literal == 1) {
        continue; // skip to next instruction, because multiplying by 1 is a no-op
      }

      instruction = &Multiply;
    }
    else if (m[1] == "div") {
      if (arg2_literal_defined && arg2_literal == 1) {
        continue; // skip to next instruction, because dividing by 1 is a no-op
      }

      instruction = &Divide;
    }
    else if (m[1] == "mod") {
      instruction = &Modulo;
    }
    else if (m[1] == "eql") {
      instruction = &Equals;
    }
    else {
      cout << "Unexpected instruction: " << m[1] << endl;
      return -1;
    }

    map<ALUState, vector<unsigned int>>* next_maxed_possible_states = new map<ALUState, vector<unsigned int>>();
    for (auto it = maxed_possible_states->begin(); it != maxed_possible_states->end(); ++it) {
      ALUState current_state = it->first;
      vector<unsigned int> current_input_list = it->second;
      // cout << "Handling current ALU state: " << current_state << endl;

      vector<ALUState> next_states;
      if (arg2 != VARIABLE_COUNT) {
        // cout << "Processing double-var instruction type..." << endl;
        next_states = current_state.PerformInstruction(instruction, arg1, arg2);
      }
      else if (arg2_literal_defined) {
        // cout << "Processing one-var one-val instruction type..." << endl;
        next_states = current_state.PerformInstruction(instruction, arg1, arg2_literal);
      }
      else {
        // cout << "Processing single-var instruction type..." << endl;
        next_states = current_state.PerformInstruction(instruction, arg1);
      }

      for (auto state:next_states) {
        vector<unsigned int> next_input_list(current_input_list);
        if (m[1] == "inp") {
          next_input_list.push_back(state.GetVariableValue(arg1));
        }

        // cout << "Next state is: " << state << " with input list:";
        // for (auto input:next_input_list) {
        //   cout << input << ",";
        // }
        // cout << endl;

        if (next_maxed_possible_states->find(state) != next_maxed_possible_states->end()) {
          if (l >= 98) {
            // cout << "Evaluating " << state << " already stored in map with input list " << ConvertDigitListToNumber(next_maxed_possible_states[state]) << " against new input list " << ConvertDigitListToNumber(next_input_list) << endl;
          }

          // If this next state has already been reached by an already-processed ALU state, then store the input list that represents the greater number
          if (ConvertDigitListToNumber(next_maxed_possible_states->at(state)) > ConvertDigitListToNumber(next_input_list)) {
            // cout << "Using the stored input list" << endl;
            next_input_list = next_maxed_possible_states->at(state);
          }
        }
        else {
          if (l >= 98) {
            // cout << "Inserting " << state << " into the map for the first time with input list " << ConvertDigitListToNumber(next_input_list) << endl;
          }
        }

        (*next_maxed_possible_states)[state] = next_input_list;
      }
    }

    // Move to next possible states for next instruction
    if (next_maxed_possible_states->size() != maxed_possible_states->size()) {
      cout << "Now handling " << maxed_possible_states->size() << " states" << endl;
    }

    delete maxed_possible_states;
    maxed_possible_states = next_maxed_possible_states;
    // cout << "end of loop" << endl;
  }

  // Pen and paper suggested answer 99994469899267, but inevitably missed something, because it's too high
  // TODO IDEAS: rather than compute everything, form strings for what the registers equal, and print out z at the end (can simplify certain instructions this way; mul x 0 means the whole expression gets wiped out and goes to 0); everything else gets prepended with (, appended with ), and then an operator written out, potentially also with () around it
  // TODO other idea: track multiple possible states like one reddit user did, combining states that are the same when possible
  // TODO idea go thru instructions in reverse order; z must be 0, so "add z y" means z and y are opposite but equal positive/negative numbers; but I don't know how to represent all possibilities for that...
  // TODO idea: try all 1s, all 2s, all 3s, etc. as input and what that gives us; or 12345.... 98765....
  // TODO HERE idea: if deleting is slow, keep a list of things that need to be deleted, and delete them later! Or figure out another way to only use 1 map the whole time (could include instruction number in ALU state, OR remove element from map as we use it - might have to use a while loop/repeatedly call begin() to get all elements to be handled correctly) - **test how iterating works while you delete; if I can guarantee order, I could insert higher ALU instructions before the current iterator, or just write skip code if state is beyond current instruction
  // TODO NEXT 1 idea: once z gets above a value so it cannot be divided by 26 out (count this from the input file), refuse to store any ALU states with z above that value limit; better yet, keep a running tally of how many div26 instructions are left, to constantly evaluate if it's worth tracking the state given its z value; could also factor in that "add x/y" instructions are remainders that get added to digit numbers, and that +9 is the max that will ever be in a /26 remainder or added to a 26 product to be valid

  // TODO FINALLY remove debugging statements

  // Determine which valid output has the greatest model number
  unsigned long long int max_model_number = 0;
  for (auto state:*maxed_possible_states) {
    if (state.first.GetVariableValue(z) == 0) {
      unsigned long long int valid_model_number = ConvertDigitListToNumber(state.second);
      if (valid_model_number > max_model_number) {
        max_model_number = valid_model_number;
      }
    }
  }

  cout << "Part 1 answer: " << max_model_number << endl;

  // Clean up memory
  delete maxed_possible_states;

  // Part 2:
  // TODO

  return 0;
}
