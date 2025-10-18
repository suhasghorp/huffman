#include "header.hpp"
#include "endian.hpp"
#include <ostream>
#include <istream>
#include <array>

namespace huffman {

namespace {
    // Magic bytes and version identifier
    constexpr std::array<char, 4> MAGIC = {'H', 'F', 'N', '1'};
    constexpr std::size_t ENTRY_SIZE = 9;  // 1 byte symbol + 8 byte frequency
}

result<void> write_header(std::ostream& os, const Header& header) {
    // Write magic bytes
    os.write(MAGIC.data(), MAGIC.size());
    if (!os) {
        return std::unexpected(Error(Error::Code::Io, "Failed to write magic bytes"));
    }
    
    // Write flags
    os.put(static_cast<char>(header.flags));
    if (!os) {
        return std::unexpected(Error(Error::Code::Io, "Failed to write flags"));
    }
    
    // Write original size (little-endian)
    write_le_u64(os, header.original_size);
    if (!os) {
        return std::unexpected(Error(Error::Code::Io, "Failed to write original size"));
    }
    
    // Write entry count (little-endian)
    if (header.entries.size() > 65535) {
        return std::unexpected(Error(Error::Code::Io, "Too many frequency entries (max 65535)"));
    }
    write_le_u16(os, static_cast<std::uint16_t>(header.entries.size()));
    if (!os) {
        return std::unexpected(Error(Error::Code::Io, "Failed to write entry count"));
    }
    
    // Write frequency entries
    for (const auto& [symbol, freq] : header.entries) {
        os.put(static_cast<char>(symbol));
        if (!os) {
            return std::unexpected(Error(Error::Code::Io, "Failed to write symbol"));
        }
        
        write_le_u64(os, freq);
        if (!os) {
            return std::unexpected(Error(Error::Code::Io, "Failed to write frequency"));
        }
    }
    
    // Write padding bits
    os.put(static_cast<char>(header.padding_bits));
    if (!os) {
        return std::unexpected(Error(Error::Code::Io, "Failed to write padding bits"));
    }
    
    return {};
}

result<Header> read_header(std::istream& is) {
    Header header;
    
    // Read and verify magic bytes
    std::array<char, 4> magic;
    is.read(magic.data(), magic.size());
    if (!is || is.gcount() != static_cast<std::streamsize>(magic.size())) {
        return std::unexpected(Error(Error::Code::Io, "Failed to read magic bytes"));
    }
    
    if (magic != MAGIC) {
        return std::unexpected(Error(Error::Code::MagicMismatch, 
            "Invalid file format - magic bytes don't match"));
    }
    
    // Read flags
    int flags_int = is.get();
    if (flags_int == EOF) {
        return std::unexpected(Error(Error::Code::Io, "Failed to read flags"));
    }
    header.flags = static_cast<byte>(flags_int);
    
    // Read original size
    if (!read_le_u64(is, header.original_size)) {
        return std::unexpected(Error(Error::Code::Io, "Failed to read original size"));
    }
    
    // Read entry count
    std::uint16_t entry_count;
    if (!read_le_u16(is, entry_count)) {
        return std::unexpected(Error(Error::Code::Io, "Failed to read entry count"));
    }
    
    // Read frequency entries
    header.entries.reserve(entry_count);
    for (std::uint16_t i = 0; i < entry_count; ++i) {
        int symbol_int = is.get();
        if (symbol_int == EOF) {
            return std::unexpected(Error(Error::Code::Io, "Failed to read symbol"));
        }
        byte symbol = static_cast<byte>(symbol_int);
        
        std::uint64_t frequency;
        if (!read_le_u64(is, frequency)) {
            return std::unexpected(Error(Error::Code::Io, "Failed to read frequency"));
        }
        
        header.entries.emplace_back(symbol, frequency);
    }
    
    // Read padding bits
    int padding_int = is.get();
    if (padding_int == EOF) {
        return std::unexpected(Error(Error::Code::Io, "Failed to read padding bits"));
    }
    header.padding_bits = static_cast<byte>(padding_int);
    
    // Validate padding bits range
    if (header.padding_bits > 7) {
        return std::unexpected(Error(Error::Code::CorruptHeader, 
            "Invalid padding bits value (must be 0-7)"));
    }
    
    return header;
}

} // namespace huffman