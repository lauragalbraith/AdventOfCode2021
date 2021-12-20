// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 18 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What is the magnitude of the final sum?
// Part 2: What is the largest magnitude of any sum of two different snailfish numbers from the homework assignment?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// SnailfishNumberElement either represents a snailfish number (a pair of elements), or a regular number (a single element)
class SnailfishNumberElement {
  private:
    int value;
    SnailfishNumberElement* parent; // track for ease of reduction operations
    SnailfishNumberElement* left;
    SnailfishNumberElement* right;

    pair<int,SnailfishNumberElement*> MaxPairDepth();
    SnailfishNumberElement* GetLeftMostSplitCandidate();
    void Split();

  public:
    static const int DEFAULT_VALUE = -1;

    SnailfishNumberElement(SnailfishNumberElement* p): value(DEFAULT_VALUE), parent(p), left(NULL), right(NULL) {}
    SnailfishNumberElement(SnailfishNumberElement* p, const int& val): value(val), parent(p), left(NULL), right(NULL) {}
    SnailfishNumberElement(const SnailfishNumberElement& orig) {
      this->value = orig.value;

      if (orig.left == NULL) {
        this->left = NULL;
      }
      else {
        this->left = new SnailfishNumberElement(*(orig.left)); // copy left recursively
        this->left->parent = this;
      }

      if (orig.right == NULL) {
        this->right = NULL;
      }
      else {
        this->right = new SnailfishNumberElement(*(orig.right)); // copy right recursively
        this->right->parent = this;
      }

      this->parent = orig.parent; // until we are told differently, our parent is the same as the original
    }
    
    static SnailfishNumberElement* ParseFromText(SnailfishNumberElement* parent, string::const_iterator start, string::const_iterator end);
    int GetValue() { return this->value; }
    void SetValue(const int& val) { this->value = val; }
    void SetParent(SnailfishNumberElement* p) { this->parent = p; }
    SnailfishNumberElement* GetParent() { return this->parent; }
    void SetLeftElement(SnailfishNumberElement* l) { this->left = l; }
    SnailfishNumberElement* GetLeftElement() { return this->left; }
    void SetRightElement(SnailfishNumberElement* r) { this->right = r; }
    SnailfishNumberElement* GetRightElement() { return this->right; }
    void Reduce();
    long long int Magnitude();
    void Print();

    ~SnailfishNumberElement() { delete this->left; delete this->right; }
};

// MaxPairDepth returns a number representing the maximum depth of the pairs, and the leftmost pair (not regular number) at that depth
// Example: for [[[[[9,8],1],2],3],4] this should return 4 and a pointer to [9,8]
pair<int,SnailfishNumberElement*> SnailfishNumberElement::MaxPairDepth() {
  // Base case: regular numbers are not eligible for being at a max depth
  if (this->left == NULL && this->right == NULL) {
    return pair<int,SnailfishNumberElement*>(-1, NULL);
  }

  pair<int,SnailfishNumberElement*> l_depth = this->left->MaxPairDepth();
  pair<int,SnailfishNumberElement*> r_depth = this->right->MaxPairDepth();
  // Secondary base case: both left/right are regular numbers - "Exploding pairs will always consist of two regular numbers"
  if (r_depth.first < 0 && l_depth.first < 0) {
    return pair<int,SnailfishNumberElement*>(0, this);
  }
  else if (r_depth.first > l_depth.first) {
    return pair<int,SnailfishNumberElement*>(1+r_depth.first, r_depth.second);
  }
  else {
    return pair<int,SnailfishNumberElement*>(1+l_depth.first, l_depth.second);
  }
}

SnailfishNumberElement* SnailfishNumberElement::GetLeftMostSplitCandidate() {
  // Base case: if this element is a regular number, return this if it's a split candidate
  if (this->value >= 10) {
    return this;
  }
  else if (this->value >= 0) {
    return NULL;
  }

  // Check to the left first for split candidates
  SnailfishNumberElement* candidate = this->left->GetLeftMostSplitCandidate();
  if (candidate != NULL) {
    return candidate;
  }

  // only if there's no split candidates to the left, check the right
  return this->right->GetLeftMostSplitCandidate();
}

void SnailfishNumberElement::Split() {
  // Create left element
  this->left = new SnailfishNumberElement(this, floor(this->value / 2.0));

  // Create right element
  this->right = new SnailfishNumberElement(this, ceil(this->value / 2.0));
  
  // Reset our own number
  this->value = SnailfishNumberElement::DEFAULT_VALUE;
}

SnailfishNumberElement* SnailfishNumberElement::ParseFromText(SnailfishNumberElement* parent, string::const_iterator start, string::const_iterator end) {
  // Base case: regular number
  if (start + 1 == end) {
    return new SnailfishNumberElement(parent, stoi(string(1, *start)));
  }

  // Initialize the current element
  string::const_iterator curr_char = start+1; // move past '['
  SnailfishNumberElement* curr_num = new SnailfishNumberElement(parent);

  // Find beginning and end of the left snailfish number
  string::const_iterator left_start = curr_char;

  int commas_needed = 0;
  while (*curr_char != ',' || commas_needed > 0) {
    if (*curr_char == '[') { ++commas_needed; }
    else if (*curr_char == ',') { --commas_needed; }
    ++curr_char;
  }
  string::const_iterator left_end = curr_char; // ','

  // Parse the left snailfish number
  auto l = SnailfishNumberElement::ParseFromText(curr_num, left_start, left_end);
  curr_num->SetLeftElement(l);

  // The beginning and end of the right snailfish number are the rest of our value; move past ',' and exclude ']'
  // Parse the right snailfish number
  auto r = SnailfishNumberElement::ParseFromText(curr_num, curr_char + 1, end - 1);
  curr_num->SetRightElement(r);

  return curr_num;
}

// Print is a function to help debug state
void SnailfishNumberElement::Print() {
  if (this->value >= 0) {
    cout << this->value;
    return;
  }
  cout << "[";
  this->left->Print();
  cout << ",";
  this->right->Print();
  cout << "]";
}

// Reduce takes action on a recently-added snailfish number until no more reduce actions are applicable
void SnailfishNumberElement::Reduce() {
  bool explode_performed, split_performed;

  do {
    // Reset state of the reduce stage
    explode_performed = false, split_performed = false;

    // Check if any pair is eligible to explode (stop at the leftmost such pair)
    pair<int,SnailfishNumberElement*> explode_candidate = this->MaxPairDepth();
    if (explode_candidate.first >= 4) {
      // Add the exploding pair's left value to the first regular number to its left
      int l_val = explode_candidate.second->GetLeftElement()->GetValue();

      // find the first regular number to its left
      SnailfishNumberElement* came_from = explode_candidate.second;
      SnailfishNumberElement* parent = explode_candidate.second->GetParent();
      SnailfishNumberElement* first_elem_to_left = NULL;
      while (parent != NULL) {
        // check if the parent's left is where we just came from
        if (parent->GetLeftElement() == came_from) {
          came_from = parent;
          parent = came_from->GetParent();
          continue;
        }

        first_elem_to_left = parent->GetLeftElement();
        break;
      }

      // If we have gone to the top of the tree without finding an element to the left, then the left value of the exploded pair gets dropped
      if (first_elem_to_left != NULL) {
        // find the rightmost regular number in this element to the left of the exploded pair
        while (first_elem_to_left->GetValue() == SnailfishNumberElement::DEFAULT_VALUE) {
          first_elem_to_left = first_elem_to_left->GetRightElement();
        }

        first_elem_to_left->SetValue(first_elem_to_left->GetValue() + l_val);
      }

      // Add the exploding pair's right value to the first regular number to its right
      int r_val = explode_candidate.second->GetRightElement()->GetValue();

      // find the first regular number to its right
      came_from = explode_candidate.second;
      parent = explode_candidate.second->GetParent();
      SnailfishNumberElement* first_elem_to_right = NULL;
      while (parent != NULL) {
        // check if the parent's right is where we just came from
        if (parent->GetRightElement() == came_from) {
          came_from = parent;
          parent = came_from->GetParent();
          continue;
        }

        first_elem_to_right = parent->GetRightElement();
        break;
      }

      // If we have gone to the top of the tree without finding an element to the right, then the right value of the exploded pair gets dropped
      if (first_elem_to_right != NULL) {
        // find the leftmost regular number in this element to the right of the exploded pair
        while (first_elem_to_right->GetValue() == SnailfishNumberElement::DEFAULT_VALUE) {
          first_elem_to_right = first_elem_to_right->GetLeftElement();
        }

        first_elem_to_right->SetValue(first_elem_to_right->GetValue() + r_val);
      }

      // Replace the exploding pair in its parent with 0
      SnailfishNumberElement* exploded_parent = explode_candidate.second->GetParent();
      SnailfishNumberElement* new_zero = new SnailfishNumberElement(exploded_parent, 0);
      if (exploded_parent->GetLeftElement() == explode_candidate.second) {
        exploded_parent->SetLeftElement(new_zero);
      }
      else {
        exploded_parent->SetRightElement(new_zero);
      }

      // cleanup memory of the exploded pair
      delete explode_candidate.second;

      // "During reduction, at most one action applies, after which the process returns to the top of the list of actions"
      explode_performed = true;
      continue;
    }

    // Check if any regular number needs to split (stop at the leftmost such number)
    SnailfishNumberElement* number_to_split = this->GetLeftMostSplitCandidate();
    if (number_to_split != NULL) {
      number_to_split->Split();
      split_performed = true;
    }
  } while (explode_performed || split_performed);
}

// Magnitude returns the magnitude of the snailfish number: "magnitude of a pair is 3 times the magnitude of its left element plus 2 times the magnitude of its right element"
long long int SnailfishNumberElement::Magnitude() {
  // Base case: "magnitude of a regular number is just that number"
  if (this->value >= 0) { return this->value; }

  return (3 * this->left->Magnitude()) + (2 * this->right->Magnitude());
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day18/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  vector<SnailfishNumberElement*> numbers_to_add;
  for (auto line:file_results.first) {
    SnailfishNumberElement* s = SnailfishNumberElement::ParseFromText(NULL, line.begin(), line.end());
    numbers_to_add.push_back(s);
  }

  // Part 1:
  SnailfishNumberElement* result = new SnailfishNumberElement(*(numbers_to_add[0]));
  
  for (int i = 1; i < numbers_to_add.size(); ++i) {
    // Create added result; connect elements of new result, and update their parent values
    SnailfishNumberElement* added = new SnailfishNumberElement(NULL);

    added->SetLeftElement(result);
    result->SetParent(added);

    SnailfishNumberElement* copy_right = new SnailfishNumberElement(*(numbers_to_add[i]));
    added->SetRightElement(copy_right);
    copy_right->SetParent(added);

    // Take reduce actions after the addition, then update the result pointer
    added->Reduce();
    result = added;
  }

  cout << "Part 1 answer: " << result->Magnitude() << endl;
  delete result;

  // Part 2:
  long long int max_magnitude = 0;
  for (int l = 0; l < numbers_to_add.size(); ++l) {
    for (int r = 0; r < numbers_to_add.size(); ++r) {
      if (l == r) { continue; } // the same number added to itself is not a candidate for the result

      // Create added result; connect elements of new result, and update their parent values
      SnailfishNumberElement* result = new SnailfishNumberElement(NULL);
      SnailfishNumberElement* copy_left = new SnailfishNumberElement(*(numbers_to_add[l]));
      SnailfishNumberElement* copy_right = new SnailfishNumberElement(*(numbers_to_add[r]));

      result->SetLeftElement(copy_left);
      copy_left->SetParent(result);

      result->SetRightElement(copy_right);
      copy_right->SetParent(result);

      // Take reduce actions after the addition
      result->Reduce();

      // Compute magnitude to see if it's the greatest so far
      long long int magnitude = result->Magnitude();
      if (magnitude > max_magnitude) { max_magnitude = magnitude; }

      // Clean up memory
      delete result;
    }
  }

  cout << "Part 2 answer: " << max_magnitude << endl;

  // Clean up memory
  for (auto s:numbers_to_add) {
    delete s;
  }

  return 0;
}
