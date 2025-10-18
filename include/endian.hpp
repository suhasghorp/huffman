#pragma once

#include "types.hpp"
#include <bit>
#include <iosfwd>

namespace huffman {

// Write little-endian values to stream
void write_le_u16(std::ostream& os, std::uint16_t value);
void write_le_u64(std::ostream& os, std::uint64_t value);

// Read little-endian values from stream
bool read_le_u16(std::istream& is, std::uint16_t& value);
bool read_le_u64(std::istream& is, std::uint64_t& value);

} // namespace huffman