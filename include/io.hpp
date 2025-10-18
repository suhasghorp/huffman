#pragma once

#include "error.hpp"
#include "types.hpp"
#include <vector>
#include <span>
#include <string_view>

namespace huffman {

// Read entire file as bytes
result<std::vector<byte>> read_file_bytes(std::string_view path);

// Write bytes to file
result<void> write_file_bytes(std::string_view path, std::span<const byte> data);

} // namespace huffman