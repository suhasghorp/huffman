#include <gtest/gtest.h>
#include "frequency.hpp"
#include "io.hpp"

using namespace huffman;

TEST(FrequencyTest, VerifyTestFileCharacterCounts) {
    // Use the test file path from CMake
    const std::string path = std::string(CMAKE_SOURCE_DIR) + "/tests/test.txt";
    
    auto freq_result = compute_frequencies_from_file(path);
    ASSERT_TRUE(freq_result.has_value()) << "Failed to compute frequencies: " << freq_result.error().message;
    
    const auto& freq = freq_result.value();
    
    // Verify the specific requirements
    EXPECT_EQ(freq[static_cast<byte>('X')], 333U) << "Character 'X' should appear 333 times";
    EXPECT_EQ(freq[static_cast<byte>('t')], 223000U) << "Character 't' should appear 223,000 times";
    
    // Additional validation: verify total file size matches actual file
    auto file_bytes_result = read_file_bytes(path);
    ASSERT_TRUE(file_bytes_result.has_value()) << "Failed to re-read file for size validation";
    
    std::uint64_t total_chars = 0;
    for (std::size_t i = 0; i < 256; ++i) {
        total_chars += freq[i];
    }
    EXPECT_EQ(total_chars, file_bytes_result.value().size()) << "Total character count should match actual file size";
    
    // Also verify expected minimum file size (Les Misérables should be substantial)
    EXPECT_GT(total_chars, 3000000U) << "File should be substantial in size (Les Misérables)";
    EXPECT_LT(total_chars, 4000000U) << "File size should be reasonable";
    
    // Log actual file size for debugging
    std::cout << "Actual file size: " << total_chars << " bytes\n";
    
    // Verify we have a reasonable number of unique characters
    std::size_t unique_chars = 0;
    for (std::size_t i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            ++unique_chars;
        }
    }
    EXPECT_GT(unique_chars, 0U) << "Should have at least some unique characters";
    EXPECT_LE(unique_chars, 256U) << "Cannot have more than 256 unique byte values";
}