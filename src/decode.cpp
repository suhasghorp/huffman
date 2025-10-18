#include "decode.hpp"
#include "io.hpp"
#include "header.hpp"
#include "huffman_tree.hpp"
#include "bitstream.hpp"
#include <sstream>
#include <algorithm>

namespace huffman {

result<DecompressStats> decompress_file(std::string_view input_path, std::string_view output_path) {
    // Step 1: Read entire compressed file
    auto input_result = read_file_bytes(input_path);
    if (!input_result) {
        return std::unexpected(input_result.error());
    }
    const auto& input_bytes = input_result.value();
    
    // Step 2: Parse header
    std::istringstream input_stream(std::string(reinterpret_cast<const char*>(input_bytes.data()), 
                                                input_bytes.size()), std::ios::binary);
    
    auto header_result = read_header(input_stream);
    if (!header_result) {
        return std::unexpected(header_result.error());
    }
    const auto& header = header_result.value();
    
    // Step 3: Rebuild frequency table from header entries
    FreqTable frequencies{};
    for (const auto& [symbol, freq] : header.entries) {
        frequencies[symbol] = freq;
    }
    
    // Step 4: Rebuild Huffman tree
    auto tree_result = build_huffman_tree(frequencies);
    if (!tree_result) {
        return std::unexpected(tree_result.error());
    }
    const auto& tree = tree_result.value();
    
    // Step 5: Calculate payload boundaries
    auto header_end_pos = input_stream.tellg();
    if (header_end_pos < 0) {
        return std::unexpected(Error(Error::Code::Io, "Failed to get stream position"));
    }
    
    std::size_t header_size = static_cast<std::size_t>(header_end_pos);
    if (header_size > input_bytes.size()) {
        return std::unexpected(Error(Error::Code::CorruptHeader, "Invalid header size"));
    }
    
    std::span<const byte> payload_bytes{input_bytes.data() + header_size, 
                                       input_bytes.size() - header_size};
    
    // Step 6: Decode payload
    std::vector<byte> decoded_bytes;
    decoded_bytes.reserve(header.original_size);
    
    if (header.original_size == 0) {
        // Empty file - write empty output and return
        auto write_result = write_file_bytes(output_path, decoded_bytes);
        if (!write_result) {
            return std::unexpected(write_result.error());
        }
        
        DecompressStats stats;
        stats.input_bytes = input_bytes.size();
        stats.output_bytes = 0;
        return stats;
    }
    
    if (!tree) {
        return std::unexpected(Error(Error::Code::DecodeFailed, 
            "Cannot decode: empty tree but non-zero original size"));
    }
    
    // Handle single-symbol tree (fast path)
    if (std::holds_alternative<Leaf>(tree->data)) {
        const auto& leaf = std::get<Leaf>(tree->data);
        decoded_bytes.resize(header.original_size, leaf.symbol);
    } else {
        // Multi-symbol tree: decode using bit reader and tree traversal
        std::size_t payload_bits = payload_bytes.size() * 8;
        if (payload_bits < header.padding_bits) {
            return std::unexpected(Error(Error::Code::CorruptHeader, 
                "Payload too small for specified padding"));
        }
        
        std::size_t total_bits = payload_bits - header.padding_bits;
        BitReader reader(payload_bytes, total_bits);
        
        while (decoded_bytes.size() < header.original_size && reader.has_bits()) {
            const Node* current = tree.get();
            
            // Walk tree until we reach a leaf
            while (std::holds_alternative<Branch>(current->data)) {
                auto bit_opt = reader.read_bit();
                if (!bit_opt) {
                    break;  // No more bits
                }
                
                const auto& branch = std::get<Branch>(current->data);
                current = bit_opt.value() ? branch.right.get() : branch.left.get();
                
                if (!current) {
                    return std::unexpected(Error(Error::Code::DecodeFailed, 
                        "Corrupt tree structure"));
                }
            }
            
            if (std::holds_alternative<Leaf>(current->data)) {
                const auto& leaf = std::get<Leaf>(current->data);
                decoded_bytes.push_back(leaf.symbol);
            }
        }
    }
    
    // Verify we decoded the expected amount
    if (decoded_bytes.size() != header.original_size) {
        return std::unexpected(Error(Error::Code::DecodeFailed, 
            "Decoded size mismatch: expected " + std::to_string(header.original_size) + 
            ", got " + std::to_string(decoded_bytes.size())));
    }
    
    // Step 7: Write decompressed data
    auto write_result = write_file_bytes(output_path, decoded_bytes);
    if (!write_result) {
        return std::unexpected(write_result.error());
    }
    
    // Return statistics
    DecompressStats stats;
    stats.input_bytes = input_bytes.size();
    stats.output_bytes = decoded_bytes.size();
    return stats;
}

} // namespace huffman