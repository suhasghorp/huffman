#include "encode.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>

using namespace huffman;

void print_usage(const std::string_view program_name) {
    std::cout << "Usage: " << program_name << " <input_file> <output_file>\n";
    std::cout << "Compress a file using Huffman coding.\n";
}

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    const std::string_view input_path = argv[1];
    const std::string_view output_path = argv[2];
    
    auto result = compress_file(input_path, output_path);
    if (!result) {
        std::cerr << "Compression failed: " << result.error().message << '\n';
        return 1;
    }
    
    const auto& stats = result.value();
    std::cout << "Compression complete:\n";
    std::cout << "  Input size:  " << stats.input_bytes << " bytes\n";
    std::cout << "  Output size: " << stats.output_bytes << " bytes\n";
    std::cout << "  Ratio:       " << std::fixed << std::setprecision(2) 
              << stats.compression_ratio() << ":1\n";
    std::cout << "  Padding:     " << static_cast<int>(stats.padding_bits) << " bits\n";
    
    return 0;
}