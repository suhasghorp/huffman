#include "frequency.hpp"
#include "io.hpp"
#include <algorithm>

namespace huffman {

FreqTable compute_frequencies(std::span<const byte> bytes) {
    FreqTable frequencies{};  // Initialize all to 0
    
    // Use STL algorithm to count frequencies
    std::for_each(bytes.begin(), bytes.end(), [&frequencies](byte b) {
        ++frequencies[b];
    });
    
    return frequencies;
}

result<FreqTable> compute_frequencies_from_file(std::string_view path) {
    auto bytes_result = read_file_bytes(path);
    if (!bytes_result) {
        return std::unexpected(bytes_result.error());
    }
    
    return compute_frequencies(bytes_result.value());
}

} // namespace huffman