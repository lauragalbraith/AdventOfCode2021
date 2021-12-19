// main.cpp: Laura Galbraith
// Description: solver for Puzzles 1 and 2 of Day 16 of The Advent Of Code 2021
// See: https://adventofcode.com/2021
// Part 1: What do you get if you add up the version numbers in all packets?
// Part 2: TODO

#include "../util/fileutil.hpp" // ReadLinesFromFile
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>

using namespace std;

vector<bool> hex_to_bin(const char& c) {
  switch(c) {
    case '0': return vector<bool>({0,0,0,0});
    case '1': return vector<bool>({0,0,0,1});
    case '2': return vector<bool>({0,0,1,0});
    case '3': return vector<bool>({0,0,1,1});
    case '4': return vector<bool>({0,1,0,0});
    case '5': return vector<bool>({0,1,0,1});
    case '6': return vector<bool>({0,1,1,0});
    case '7': return vector<bool>({0,1,1,1});
    case '8': return vector<bool>({1,0,0,0});
    case '9': return vector<bool>({1,0,0,1});
    case 'A': return vector<bool>({1,0,1,0});
    case 'B': return vector<bool>({1,0,1,1});
    case 'C': return vector<bool>({1,1,0,0});
    case 'D': return vector<bool>({1,1,0,1});
    case 'E': return vector<bool>({1,1,1,0});
    case 'F': return vector<bool>({1,1,1,1});

    default: return vector<bool>();
  }
}

// ParseBytesAsInt will iterate over the bytes pointed to between the given iterators (most significant bit first) and return the represented binary number
unsigned int ParseBytesAsInt(vector<bool>::const_iterator current, const vector<bool>::const_iterator& end) {
  double num_bytes = end - current;
  if (num_bytes <= 0) {
    throw invalid_argument("current iterator must be before end iterator");
  }

  unsigned int decimal = 0;
  double current_base = pow(2.0, num_bytes - 1);

  while (current != end) {
    decimal += *current * current_base;

    current += 1;
    current_base = current_base / 2.0;
  }
  
  return decimal;
}

// 100 (4) is a literal value packet; any other type ID represents an operator
enum PacketType { op = 0, literal_val = 4 };

class Packet {
  protected:
    const unsigned int version;
    const PacketType type;
  public:
    Packet(const unsigned int& ver, const PacketType& type_id) : version(ver), type(type_id) {}
    unsigned int GetVersion() { return this->version; }
    PacketType GetType() { return this->type; }
    virtual unsigned int SumPacketAndSubPacketVersions() { return 0; }
    virtual ~Packet() {} // must be made virtual so that we can delete any packet using the derived class's destructor
};

pair<Packet*,vector<bool>::const_iterator> ParseSinglePacket(vector<bool>::const_iterator start, const vector<bool>::const_iterator& limit);

class LiteralValuePacket : public Packet {
  private:
    unsigned int value;
  public:
    LiteralValuePacket(const unsigned int& val, const unsigned int& ver) : Packet(ver,literal_val), value(val) {}
    unsigned int GetValue() { return this->value; }
    unsigned int SumPacketAndSubPacketVersions() { return this->version; }
    ~LiteralValuePacket() {} // No internal memory usage to clear
    static pair<LiteralValuePacket*,vector<bool>::const_iterator> ParseLiteralValuePacket(
      vector<bool>::const_iterator start,
      const vector<bool>::const_iterator& end);
};

// ParseLiteralValuePacket will iterate over the bytes pointed to between the given iterators (most significant bit first) and return the represented literal value packet
// The packet encodes a single binary number, padded with leading zeroes until its length is a multiple of four bits; each group of four bits is prefixed by a 1 bit except the last group, which is prefixed by a 0 bit; ex. 110 (version) 100 (type ID) 10111 (0111 is the first bits of the value) 11110 (1110 is the next bits of the value) 00101 (0101 is the last bits of the value) 000 (just padding zeroes)
pair<LiteralValuePacket*,vector<bool>::const_iterator> LiteralValuePacket::ParseLiteralValuePacket(
  vector<bool>::const_iterator start,
  const vector<bool>::const_iterator& end)
{
  vector<bool>::const_iterator current = start;

  // Parse header
  unsigned int version = ParseBytesAsInt(current, current+3);
  current += 6; // skip type parsing, as only one value is for literal values
  cout << "Version of literal value packet: " << version << endl;

  // Despite the rules mentioning leading zeros (padding until length is 4x bits), this does not exist anywhere in the input

  // Parse value...
  vector<bool> value_bits;

  // ...parse all groups but last
  while (*current == 1) {
    value_bits.insert(value_bits.end(),current+1, current+5);
    current += 5;
  }

  // ...parse last group
  value_bits.insert(value_bits.end(),current+1, current+5);
  current += 5;

  unsigned int value = ParseBytesAsInt(value_bits.begin(), value_bits.end());

  // Form operator packet object
  LiteralValuePacket* lv_p = new LiteralValuePacket(value, version);

  // Despite the rules mentioning "0 bits at the end are extra due to the hexadecimal representation and should be ignored", there are no such examples of this, where the trailing 0s could be considered at the end of the outermost packet

  return pair<LiteralValuePacket*,vector<bool>::const_iterator>(lv_p, current);
}

class OperatorPacket : public Packet {
  private:
    vector<Packet*> contained_packets;
  public:
    OperatorPacket(const unsigned int& ver, const vector<Packet*>& sub_packets) : Packet(ver,op) {
      this->contained_packets.resize(sub_packets.size());
      for (int i = 0; i < sub_packets.size(); ++i) {
        this->contained_packets[i] = sub_packets[i];
      }
    }

    void AppendContainedPacket(Packet* p) { this->contained_packets.push_back(p); } // TODO FINALLY remove this if my code can instead set all the contained packets in the object constructor
    static pair<OperatorPacket*,vector<bool>::const_iterator> ParseOperatorPacket(
      vector<bool>::const_iterator current,
      const vector<bool>::const_iterator& end);
    vector<Packet*> GetContainedPackets() { return this->contained_packets; }
    unsigned int SumPacketAndSubPacketVersions() {
      unsigned int total = this->version;
      for (auto p:this->contained_packets) {
        total += p->SumPacketAndSubPacketVersions();
      }
      return total;
    }

    ~OperatorPacket() {
      for (int i = 0; i < this->contained_packets.size(); ++i) {
        delete this->contained_packets[i];
        this->contained_packets[i] = NULL;
      }
      this->contained_packets.resize(0);
    }
};

// ParseOperatorPacket will iterate over the bytes pointed to between the given iterators (most significant bit first) and return the represented operator packet
// After the version/type ID, it has a length type ID bit (0: next 15 bits are the total length in bits of the sub-packets in the operator; 1: next 11 bits are the number of sub-packets immediately contained by the operator), then the sub-packets
pair<OperatorPacket*,vector<bool>::const_iterator> OperatorPacket::ParseOperatorPacket(
  vector<bool>::const_iterator start,
  const vector<bool>::const_iterator& end)
{
  vector<bool>::const_iterator current = start;

  // Parse header
  unsigned int version = ParseBytesAsInt(current, current+3);
  current += 3;
  unsigned int type_id = ParseBytesAsInt(current, current+3);
  current += 3;
  PacketType type = static_cast<PacketType>(type_id);
  cout << "Version of operator packet: " << version << ", Type: " << type << endl;

  // Parse length type ID
  bool length_type_id = *current;
  current += 1;
  cout << "Length type ID: " << length_type_id << endl;
  
  // Parse the length bits and sub-packets
  vector<Packet*> sub_packets;
  if (length_type_id == 0) { // 15 bits are the length in bits of the sub-packets
    // Note: this length does not account for leading zeroes at the end of the operator packet
    unsigned int bit_length_sub = ParseBytesAsInt(current, current + 15);
    current += 15;
    cout << "Length in bits of the sub-packets: " << bit_length_sub << " compared to length until end: " << (end - current) << endl;
    vector<bool>::const_iterator sub_packets_limit = current + bit_length_sub;

    // parse sub-packets
    do {
      pair<Packet*,vector<bool>::const_iterator> packet_result = ParseSinglePacket(current, sub_packets_limit);

      sub_packets.push_back(packet_result.first);
      current = packet_result.second;
    } while (current < sub_packets_limit);
  }
  else { // 11 bits are the number of sub-packets
    // Note: this number is only the number of sub-packets, not sub-sub-packets (ex. if this operator contains an operator which contains a literal, the number of sub-packets of this operator will be 1)
    unsigned int num_sub_packets = ParseBytesAsInt(current, current + 11);
    current += 11;
    cout << "Number of sub-packets: " << num_sub_packets << endl;
    sub_packets.resize(num_sub_packets);

    // parse sub_packets
    for (int i = 0; i < num_sub_packets; ++i) {
      pair<Packet*,vector<bool>::const_iterator> packet_result = ParseSinglePacket(current, end);

      sub_packets[i] = packet_result.first;
      current = packet_result.second;
    }
  }

  // Form operator packet object
  OperatorPacket* op_p = new OperatorPacket(version, sub_packets); // TODO PART 2: if type matters beyond operator/literal, pass it in here

  // Despite the rules mentioning trailing zero-padding in packets, none exists except in the outermost packet

  return pair<OperatorPacket*,vector<bool>::const_iterator>(op_p, current);
}

// ParseSinglePacket calls the parsing function depends on the packet type
// If there's any remaining bits after the single packet is processed, return the iterator to the start of the remaining bits
pair<Packet*,vector<bool>::const_iterator> ParseSinglePacket(vector<bool>::const_iterator start, const vector<bool>::const_iterator& limit) {
  // Parse packet type ID to determine which parsing function to call
  unsigned int type_id = ParseBytesAsInt(start+3, start+6);
  if (type_id == literal_val) {
    return LiteralValuePacket::ParseLiteralValuePacket(start, limit);
  }
  else {
    return OperatorPacket::ParseOperatorPacket(start, limit);
  }
}

int main() {
  pair<vector<string>, int> file_results = ReadLinesFromFile("day16/input.txt");
  if (file_results.second < 0) {
    cout << "Failed to read file" << endl;
    return -1;
  }

  // Parse file input
  // Since we don't have a static number of bytes, we will use vector<bool> instead of <bitste>: https://www.cplusplus.com/reference/vector/vector-bool/
  vector<bool> input;
  for (int i = 0; i < file_results.first[0].size(); ++i) {
    // Convert the hexadecimal input into binary
    vector<bool> binary_four = hex_to_bin(file_results.first[0][i]);
    input.insert(input.end(), binary_four.begin(), binary_four.end());
  }

  cout << "Input:" << endl; // TODO FINALLY remove all debug statements
  for (auto b:input) {
    cout << b;
  }
  cout << endl << endl;

  // Part 1:
  // Since we know that the whole BITS transmission is a single packet "which itself contains many other packets", it must be an operator type
  pair<OperatorPacket*,vector<bool>::const_iterator> packet_result = OperatorPacket::ParseOperatorPacket(input.begin(), input.end());

  // Move thru trailing zeroes at the end of the outermost packet
  vector<bool>::const_iterator current = packet_result.second;
  while (current < input.end() && *current == 0) {
    ++current;
  }
  if (current != input.end()) {
    cout << "Packet parsing did not continue to the end of the input; it was " << (input.end() - packet_result.second) << " bits off" << endl;
    return -1;
  }

  Packet* outer_packet = packet_result.first;
  cout << "Part 1 answer: " << outer_packet->SumPacketAndSubPacketVersions() << endl;

  // Part 2:
  // TODO

  // Free all memory used
  delete outer_packet; // this will recursively clean up all contained sub-packets

  return 0;
}
