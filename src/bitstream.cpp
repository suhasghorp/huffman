#include "bitstream.hpp"
#include <algorithm>

namespace huffman {

// BitWriter implementation

void BitWriter::push_bit(bool b) {
    if (b) {
        current_ |= (1 << bit_pos_);  // Set bit at position
    }
    
    ++bit_pos_;
    
    if (bit_pos_ == 8) {
        // Byte complete - store and reset
        bytes_.push_back(current_);
        current_ = 0;
        bit_pos_ = 0;
    }
}

void BitWriter::push_bits(std::span<const byte> bits) {
    std::for_each(bits.begin(), bits.end(), [this](byte b) {
        push_bit(b != 0);
    });
}

byte BitWriter::flush() {
    if (bit_pos_ == 0) {
        return 0;  // No padding needed
    }
    
    // Store the partial byte (remaining bits are 0)
    bytes_.push_back(current_);
    
    auto padding_bits = static_cast<byte>(8 - bit_pos_);
    current_ = 0;
    bit_pos_ = 0;
    
    return padding_bits;
}

void BitWriter::clear() {
    bytes_.clear();
    current_ = 0;
    bit_pos_ = 0;
}

// BitReader implementation

std::optional<bool> BitReader::read_bit() {
    if (bits_read_ >= total_bits_) {
        return std::nullopt;
    }
    
    std::size_t byte_index = bits_read_ / 8;
    std::size_t bit_index = bits_read_ % 8;
    
    if (byte_index >= bytes_.size()) {
        return std::nullopt;
    }
    
    bool bit_value = (bytes_[byte_index] & (1 << bit_index)) != 0;
    ++bits_read_;
    
    return bit_value;
}

} // namespace huffman