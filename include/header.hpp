#pragma once

#include "error.hpp"
#include "types.hpp"
#include <vector>
#include <utility>
#include <iosfwd>

namespace huffman {

// Header structure for compressed files
struct Header {
    std::uint64_t original_size;     // Original uncompressed size
    std::vector<std::pair<byte, std::uint64_t>> entries;  // (symbol, frequency) pairs
    byte padding_bits;               // Number of unused bits in final byte (0-7)
    byte flags;                      // Reserved for future use (0 for now)
    
    Header() : original_size(0), padding_bits(0), flags(0) {}
};

// Write header to output stream
result<void> write_header(std::ostream& os, const Header& header);

// Read header from input stream
result<Header> read_header(std::istream& is);

} // namespace huffman