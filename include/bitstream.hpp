#pragma once

#include "types.hpp"
#include <vector>
#include <span>
#include <optional>

namespace huffman {

// Write bits to a buffer
class BitWriter {
private:
    std::vector<byte> bytes_;
    byte current_ = 0;
    byte bit_pos_ = 0;  // Position within current byte (0-7)

public:
    // Write a single bit
    void push_bit(bool b);
    
    // Write multiple bits
    void push_bits(std::span<const byte> bits);
    
    // Flush remaining bits and return padding count
    byte flush();
    
    // Get the current buffer
    std::span<const byte> buffer() const { return bytes_; }
    
    // Reset for reuse
    void clear();
};

// Read bits from a buffer  
class BitReader {
private:
    std::span<const byte> bytes_;
    std::size_t total_bits_;
    std::size_t bits_read_ = 0;

public:
    BitReader(std::span<const byte> bytes, std::size_t total_bits)
        : bytes_(bytes), total_bits_(total_bits) {}
    
    // Read next bit, or nullopt if no more bits
    std::optional<bool> read_bit();
    
    // Check if more bits available
    bool has_bits() const { return bits_read_ < total_bits_; }
};

} // namespace huffman