// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 20 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: Applying the image enhancement algorithm twice, how many pixels are lit in the resulting image?
// Part 2: Applying the image enhancement algorithm 50 times, how many pixels are lit in the resulting image?

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <cmath>

using namespace std;

class Pixel {
  private:
    static const char lit_char = '#';
    static const char unlit_char = '.';

    bool lit;

  public:
    Pixel(); // default constructor required for resizing
    Pixel(const char& c);
    Pixel(const Pixel& other): lit(other.lit) {} // copy constructor

    const bool IsLit() const { return this->lit; }
    const char ToChar() const;
    const double ToBit() const { return this->lit ? 1 : 0; }

    ~Pixel() {}
};

// "the rest of the input image consists of dark pixels (.)"
Pixel::Pixel() {
  this->lit = false;
} 

Pixel::Pixel(const char& c) {
  switch(c) {
    case Pixel::lit_char: {
      this->lit = true;
      break;
    }
    case Pixel::unlit_char: {
      this->lit = false;
      break;
    }

    default: {
      stringstream err;
      err << "invalid pixel initialization character: " << c;
      throw invalid_argument(err.str());
    }
  }
}

const char Pixel::ToChar() const {
  return (this->lit ? Pixel::lit_char : Pixel::unlit_char);
}

class Image {
  private:
    static const int EDGE_SIZE = 1;

    vector<vector<Pixel>> pixels_and_edge; // acessed with [row][col]; starts at [0][0]
    Pixel outer_reach_val;

    static vector<pair<int,int>> GetSurroundingSquareCoordinates(const pair<int,int>& center);
    const Pixel GetPixelValue(const pair<int,int>& coordinate) const;

  public:
    Image(const vector<vector<Pixel>>& input);
    Image(const Image* previous_state);

    void Print();
    int LitPixels() const;
    void Enhance(const vector<Pixel>& algorithm);

    ~Image() {}
};

// This constructor will initialize all the way out to the edge; it will not add any buffer elements
Image::Image(const vector<vector<Pixel>>& input) {
  // Copy pixel values
  this->pixels_and_edge = input;

  // Start the outer reach pixel value as the default Pixel value
  this->outer_reach_val = Pixel();
}

// Print is a utility function to ease debugging
void Image::Print() {
  for (auto row:this->pixels_and_edge) {
    for (auto col:row) {
      cout << col.ToChar();
    }
    cout << endl;
  }
}

// LitPixels returns the number of lit pixels in the image
// If the outer-reach pixels are defined as lit, then there are an infinite number of lit pixels, and -1 will be returned
int Image::LitPixels() const {
  if (this->outer_reach_val.IsLit()) {
    return -1;
  }

  int lit_count = 0;
  for (auto row:this->pixels_and_edge) {
    for (auto col:row) {
      if (col.IsLit()) { ++lit_count; }
    }
  }

  return lit_count;
}

// GetSurroundingSquareCoordinates returns the ordered list of the coordinates of each pixel in the square centered around the given point, defined by the edge size
vector<pair<int,int>> Image::GetSurroundingSquareCoordinates(const pair<int,int>& center) {
  vector<pair<int,int>> ret;
  for (int row_add = -1 * Image::EDGE_SIZE; row_add <= Image::EDGE_SIZE; ++row_add) {
    for (int col_add = -1 * Image::EDGE_SIZE; col_add <= Image::EDGE_SIZE; ++col_add) {
      ret.push_back(pair<int,int>(center.first + row_add, center.second + col_add));
    }
  }

  return ret;
}

// GetPixelValue returns the pixel value at the given row,col values, according to the internal storage of the image state
const Pixel Image::GetPixelValue(const pair<int,int>& coordinate) const {
  if (coordinate.first < 0
    || coordinate.first >= this->pixels_and_edge.size()
    || coordinate.second < 0
    || coordinate.second >= this->pixels_and_edge[coordinate.first].size()) {
      return this->outer_reach_val;
    }
  
  return this->pixels_and_edge[coordinate.first][coordinate.second];
}

// Enhance converts all pixels in the input image using the given algorithm
void Image::Enhance(const vector<Pixel>& algorithm) {
  // Create on-the-fly storage for determining new values
  vector<vector<Pixel>> output; // represents pixels_and_edge with another edge around it
  output.resize(this->pixels_and_edge.size() + 2*Image::EDGE_SIZE);
  for (int row = 0; row < output.size(); ++row) {
    output[row].resize(this->pixels_and_edge[0].size() + 2*Image::EDGE_SIZE);
    for (int col = 0; col < output[row].size(); ++col) {
      output[row][col] = Pixel();
    }
  }

  // Run algorithm over input to fill in output image
  for (int output_row = 0; output_row < output.size(); ++output_row) {
    for (int output_col = 0; output_col < output[output_row].size(); ++output_col) {
      const int input_row = output_row - Image::EDGE_SIZE;
      const int input_col = output_col - Image::EDGE_SIZE;

      // Get coordinates of the 9 pixels in the square centered around this pixel, in order
      vector<pair<int,int>> square_coordinates = Image::GetSurroundingSquareCoordinates(pair<int,int>(input_row, input_col));

      // Create binary number with pixel values in order
      double algo_idx = 0;

      for (int i = 0; i < square_coordinates.size(); ++i) {
        // get pixel value in binary
        double bit_val = this->GetPixelValue(square_coordinates[i]).ToBit();

        algo_idx += bit_val * pow(2, square_coordinates.size() - i - 1);
      }

      // Index into the algorithm to determine the output pixel
      output[output_row][output_col] = Pixel(algorithm[algo_idx]);
    }
  }

  // Save result
  this->pixels_and_edge = output;

  // Calculate what the value of the rest of the infinite field is now
  double outer_reach_algo_idx = this->outer_reach_val.IsLit() ? pow(2, pow(1 + 2*Image::EDGE_SIZE, 2)) - 1 : 0;
  this->outer_reach_val = Pixel(algorithm[outer_reach_algo_idx]);
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day20/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse image enhancement algorithm
  if (file_results.first[0].size() != 512) {
    cout << "Invalid length of image enhancement algorithm string: " << file_results.first[0].size() << endl;
    return -1;
  }

  vector<Pixel> image_enhancement_algorithm;
  image_enhancement_algorithm.resize(file_results.first[0].size());
  for (int i = 0; i < file_results.first[0].size(); ++i) {
    image_enhancement_algorithm[i] = Pixel(file_results.first[0][i]);
  }

  // Parse input image: light/dark pixels
  vector<vector<Pixel>> initial_input_image; // accessed with [row][col]
  for (int l = 2; l < file_results.first.size(); ++l) {
    vector<Pixel> row;
    row.resize(file_results.first[l].size());
    for (int i = 0; i < file_results.first[l].size(); ++i) {
      row[i] = Pixel(file_results.first[l][i]);
    }
    initial_input_image.push_back(row);
  }

  // Create initial image state
  Image* image_state = new Image(initial_input_image);

  // Part 1:
  for (int application = 1; application <= 2; ++application) {
    // Apply image enhancement algorithm
    image_state->Enhance(image_enhancement_algorithm);
  }

  cout << "Part 1 answer: " << image_state->LitPixels() << endl;

  // Part 2:
  for (int application = 3; application <= 50; ++application) {
    // Apply image enhancement algorithm
    image_state->Enhance(image_enhancement_algorithm);
  }

  cout << "Part 2 answer: " << image_state->LitPixels() << endl;

  // Clean up memory
  delete image_state;

  return 0;
}
