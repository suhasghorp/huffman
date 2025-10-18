#pragma once

#include "error.hpp"
#include <string_view>
#include <cstddef>

namespace huffman {

// Statistics from decompression operation
struct DecompressStats {
    std::size_t input_bytes;
    std::size_t output_bytes;
};

// Decompress a file using Huffman coding
result<DecompressStats> decompress_file(std::string_view input_path, std::string_view output_path);

} // namespace huffman