#pragma once

#include "error.hpp"
#include "types.hpp"
#include <string_view>
#include <cstddef>

namespace huffman {

// Statistics from compression operation
struct CompressStats {
    std::size_t input_bytes;
    std::size_t output_bytes;
    byte padding_bits;
    
    double compression_ratio() const {
        return output_bytes == 0 ? 0.0 : 
               static_cast<double>(input_bytes) / static_cast<double>(output_bytes);
    }
};

// Compress a file using Huffman coding
result<CompressStats> compress_file(std::string_view input_path, std::string_view output_path);

} // namespace huffman