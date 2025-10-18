#include "decode.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>

using namespace huffman;

void print_usage(const std::string_view program_name) {
    std::cout << "Usage: " << program_name << " <input_file> <output_file>\n";
    std::cout << "Decompress a file using Huffman coding.\n";
}

int main(const int argc, char* argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    const std::string_view input_path = argv[1];
    const std::string_view output_path = argv[2];
    
    auto result = decompress_file(input_path, output_path);
    if (!result) {
        std::cerr << "Decompression failed: " << result.error().message << '\n';
        return 1;
    }
    
    const auto&[input_bytes, output_bytes] = result.value();
    std::cout << "Decompression complete:\n";
    std::cout << "  Input size:  " << input_bytes << " bytes\n";
    std::cout << "  Output size: " << output_bytes << " bytes\n";
    
    return 0;
}