#include <gtest/gtest.h>
#include "frequency.hpp"

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
    
    // Additional validation: verify total file size
    std::uint64_t total_chars = 0;
    for (std::size_t i = 0; i < 256; ++i) {
        total_chars += freq[i];
    }
    EXPECT_EQ(total_chars, 3369045U) << "Total character count should match file size";
    
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