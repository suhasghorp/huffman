#include "endian.hpp"
#include <ostream>
#include <istream>

namespace huffman {

void write_le_u16(std::ostream& os, std::uint16_t value) {
    if constexpr (std::endian::native == std::endian::big) {
        value = std::byteswap(value);
    }
    os.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

void write_le_u64(std::ostream& os, std::uint64_t value) {
    if constexpr (std::endian::native == std::endian::big) {
        value = std::byteswap(value);
    }
    os.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

bool read_le_u16(std::istream& is, std::uint16_t& value) {
    is.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (!is || is.gcount() != sizeof(value)) {
        return false;
    }
    if constexpr (std::endian::native == std::endian::big) {
        value = std::byteswap(value);
    }
    return true;
}

bool read_le_u64(std::istream& is, std::uint64_t& value) {
    is.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (!is || is.gcount() != sizeof(value)) {
        return false;
    }
    if constexpr (std::endian::native == std::endian::big) {
        value = std::byteswap(value);
    }
    return true;
}

} // namespace huffman