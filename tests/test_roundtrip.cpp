#include <gtest/gtest.h>
#include "encode.hpp"
#include "decode.hpp"
#include "io.hpp"
#include "header.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace huffman;

class RoundtripTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directory for test outputs
        std::filesystem::create_directories(temp_dir_);
    }
    
    void TearDown() override {
        // Clean up temporary files
        std::error_code ec;
        std::filesystem::remove_all(temp_dir_, ec);
        // Ignore cleanup errors
    }
    
    const std::string temp_dir_ = std::string(CMAKE_BINARY_DIR) + "/tmp";
    const std::string input_path_ = std::string(CMAKE_SOURCE_DIR) + "/tests/test.txt";
    const std::string compressed_path_ = temp_dir_ + "/test.huf";
    const std::string decompressed_path_ = temp_dir_ + "/test.dec";
};

TEST_F(RoundtripTest, CompressAndDecompressLessMiserables) {
    // Step 1: Compress the original file
    auto compress_result = compress_file(input_path_, compressed_path_);
    ASSERT_TRUE(compress_result.has_value()) 
        << "Compression failed: " << compress_result.error().message;
    
    const auto& compress_stats = compress_result.value();
    EXPECT_GT(compress_stats.input_bytes, 0U) << "Input should not be empty";
    EXPECT_GT(compress_stats.output_bytes, 0U) << "Output should not be empty";
    EXPECT_LE(compress_stats.padding_bits, 7U) << "Padding should be 0-7 bits";
    
    // Verify compression achieved some reduction (not guaranteed, but expected for Les Misérables)
    std::cout << "Compression ratio: " << compress_stats.compression_ratio() << ":1\n";
    
    // Step 2: Decompress the compressed file
    auto decompress_result = decompress_file(compressed_path_, decompressed_path_);
    ASSERT_TRUE(decompress_result.has_value()) 
        << "Decompression failed: " << decompress_result.error().message;
    
    const auto&[input_bytes, output_bytes] = decompress_result.value();
    EXPECT_EQ(output_bytes, compress_stats.input_bytes)
        << "Decompressed size should match original size";
    
    // Step 3: Read both original and decompressed files
    auto original_result = read_file_bytes(input_path_);
    ASSERT_TRUE(original_result.has_value()) 
        << "Failed to read original file: " << original_result.error().message;
    
    auto decompressed_result = read_file_bytes(decompressed_path_);
    ASSERT_TRUE(decompressed_result.has_value()) 
        << "Failed to read decompressed file: " << decompressed_result.error().message;
    
    const auto& original_bytes = original_result.value();
    const auto& decompressed_bytes = decompressed_result.value();
    
    // Step 4: Verify exact byte-for-byte equality
    EXPECT_EQ(original_bytes.size(), decompressed_bytes.size()) 
        << "File sizes don't match";
    
    EXPECT_EQ(original_bytes, decompressed_bytes) 
        << "File contents don't match exactly";
    
    // Step 5: Verify header information by parsing compressed file
    auto compressed_data_result = read_file_bytes(compressed_path_);
    ASSERT_TRUE(compressed_data_result.has_value());
    
    std::istringstream compressed_stream(
        std::string(reinterpret_cast<const char*>(compressed_data_result.value().data()),
                   compressed_data_result.value().size()), 
        std::ios::binary);
    
    auto header_result = read_header(compressed_stream);
    ASSERT_TRUE(header_result.has_value()) 
        << "Failed to parse header: " << header_result.error().message;
    
    const auto& header = header_result.value();
    EXPECT_EQ(header.original_size, original_bytes.size()) 
        << "Header original_size should match actual original size";
    EXPECT_LE(header.padding_bits, 7U) 
        << "Header padding_bits should be 0-7";
    EXPECT_GT(header.entries.size(), 0U) 
        << "Header should contain frequency entries";
}

TEST_F(RoundtripTest, HandleEmptyFile) {
    // Create an empty input file
    const std::string empty_input = temp_dir_ + "/empty.txt";
    const std::string empty_compressed = temp_dir_ + "/empty.huf";
    const std::string empty_decompressed = temp_dir_ + "/empty.dec";
    
    std::ofstream empty_file(empty_input, std::ios::binary);
    empty_file.close();
    
    // Compress empty file
    auto compress_result = compress_file(empty_input, empty_compressed);
    ASSERT_TRUE(compress_result.has_value()) 
        << "Empty file compression failed: " << compress_result.error().message;
    
    EXPECT_EQ(compress_result.value().input_bytes, 0U);
    
    // Decompress empty file
    auto decompress_result = decompress_file(empty_compressed, empty_decompressed);
    ASSERT_TRUE(decompress_result.has_value()) 
        << "Empty file decompression failed: " << decompress_result.error().message;
    
    EXPECT_EQ(decompress_result.value().output_bytes, 0U);
    
    // Verify empty output file
    auto decompressed_result = read_file_bytes(empty_decompressed);
    ASSERT_TRUE(decompressed_result.has_value());
    EXPECT_TRUE(decompressed_result.value().empty());
}

TEST_F(RoundtripTest, HandleSingleSymbolFile) {
    // Create a file with only one repeated character
    const std::string single_input = temp_dir_ + "/single.txt";
    const std::string single_compressed = temp_dir_ + "/single.huf";
    const std::string single_decompressed = temp_dir_ + "/single.dec";
    
    constexpr std::size_t repeat_count = 1000;
    std::ofstream single_file(single_input, std::ios::binary);
    for (std::size_t i = 0; i < repeat_count; ++i) {
        single_file.put('A');
    }
    single_file.close();
    
    // Compress single-symbol file
    auto compress_result = compress_file(single_input, single_compressed);
    ASSERT_TRUE(compress_result.has_value()) 
        << "Single symbol compression failed: " << compress_result.error().message;
    
    EXPECT_EQ(compress_result.value().input_bytes, repeat_count);
    
    // Decompress single-symbol file
    auto decompress_result = decompress_file(single_compressed, single_decompressed);
    ASSERT_TRUE(decompress_result.has_value()) 
        << "Single symbol decompression failed: " << decompress_result.error().message;
    
    EXPECT_EQ(decompress_result.value().output_bytes, repeat_count);
    
    // Verify content
    auto original_result = read_file_bytes(single_input);
    auto decompressed_result = read_file_bytes(single_decompressed);
    ASSERT_TRUE(original_result.has_value());
    ASSERT_TRUE(decompressed_result.has_value());
    
    EXPECT_EQ(original_result.value(), decompressed_result.value());
}