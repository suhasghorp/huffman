#pragma once

#include <cstdint>
#include <array>

namespace huffman {

// Frequency table for all 256 possible byte values
using FreqTable = std::array<std::uint64_t, 256>;

// Byte type for clarity
using byte = std::uint8_t;

} // namespace huffman