#pragma once

#include "error.hpp"
#include "types.hpp"
#include <span>
#include <string_view>

namespace huffman {

// Compute frequency table from raw bytes
FreqTable compute_frequencies(std::span<const byte> bytes);

// Compute frequency table from file
result<FreqTable> compute_frequencies_from_file(std::string_view path);

} // namespace huffman