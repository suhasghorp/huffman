#pragma once

#include "huffman_tree.hpp"
#include "types.hpp"
#include <vector>
#include <array>
#include <algorithm>

namespace huffman {

// Code representation as sequence of bits (0 or 1)
struct Code {
    std::vector<byte> bits;  // Each element is 0 or 1
    
    Code() = default;
    Code(std::vector<byte> b) : bits(std::move(b)) {}
};

// Complete code table for all 256 possible byte values
struct CodeTable {
    std::array<Code, 256> codes;
    std::array<byte, 256> lengths;  // Cache of bits.size() for efficiency
    
    CodeTable() {
        // Initialize all lengths to 0
        std::fill(lengths.begin(), lengths.end(), byte{0});
    }
};

// Generate prefix code table from Huffman tree
CodeTable generate_code_table(const Node* root);

} // namespace huffman