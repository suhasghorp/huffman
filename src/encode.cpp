#include "encode.hpp"
#include "io.hpp"
#include "frequency.hpp"
#include "huffman_tree.hpp"
#include "code_table.hpp"
#include "bitstream.hpp"
#include "header.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace huffman {

result<CompressStats> compress_file(std::string_view input_path, std::string_view output_path) {
    // Step 1: Read input file
    auto input_result = read_file_bytes(input_path);
    if (!input_result) {
        return std::unexpected(input_result.error());
    }
    const auto& input_bytes = input_result.value();
    
    // Step 2: Compute character frequencies
    auto frequencies = compute_frequencies(input_bytes);
    
    // Step 3: Build Huffman tree
    auto tree_result = build_huffman_tree(frequencies);
    if (!tree_result) {
        return std::unexpected(tree_result.error());
    }
    const auto& tree = tree_result.value();
    
    // Step 4: Generate code table
    auto code_table = generate_code_table(tree.get());
    
    // Step 5: Encode data using BitWriter
    BitWriter writer;
    
    // Use STL algorithm to encode each byte
    std::for_each(input_bytes.begin(), input_bytes.end(), [&](byte b) {
        if (code_table.lengths[b] > 0) {
            writer.push_bits(code_table.codes[b].bits);
        }
    });
    
    auto padding_bits = writer.flush();
    auto compressed_payload = writer.buffer();
    
    // Step 6: Build header with frequency entries (only non-zero frequencies)
    Header header;
    header.original_size = input_bytes.size();
    header.padding_bits = padding_bits;
    header.flags = 0;
    
    // Collect non-zero frequency entries
    for (std::size_t i = 0; i < 256; ++i) {
        if (frequencies[i] > 0) {
            header.entries.emplace_back(static_cast<byte>(i), frequencies[i]);
        }
    }
    
    // Step 7: Write output file
    std::ofstream output_file(std::filesystem::path{output_path}, std::ios::binary);
    if (!output_file) {
        return std::unexpected(Error(Error::Code::Io, 
            std::string("Failed to open output file: ") + std::string(output_path)));
    }
    
    // Write header
    auto header_result = write_header(output_file, header);
    if (!header_result) {
        return std::unexpected(header_result.error());
    }
    
    // Write compressed payload
    if (!compressed_payload.empty()) {
        output_file.write(reinterpret_cast<const char*>(compressed_payload.data()), 
                         static_cast<std::streamsize>(compressed_payload.size()));
        if (!output_file) {
            return std::unexpected(Error(Error::Code::Io, "Failed to write compressed data"));
        }
    }
    
    output_file.close();
    if (!output_file) {
        return std::unexpected(Error(Error::Code::Io, "Failed to close output file"));
    }
    
    // Calculate total output size
    std::size_t output_size = 0;
    output_size += 4; // magic
    output_size += 1; // flags  
    output_size += 8; // original_size
    output_size += 2; // entry_count
    output_size += header.entries.size() * 9; // entries (1 + 8 bytes each)
    output_size += 1; // padding_bits
    output_size += compressed_payload.size(); // payload
    
    // Step 8: Return statistics
    CompressStats stats;
    stats.input_bytes = input_bytes.size();
    stats.output_bytes = output_size;
    stats.padding_bits = padding_bits;
    
    return stats;
}

} // namespace huffman