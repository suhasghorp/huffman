# Huffman Compression Tool

[![CI](https://github.com/USERNAME/huffman/actions/workflows/ci.yml/badge.svg)](https://github.com/USERNAME/huffman/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A complete implementation of Huffman coding in C++23 with no exceptions, strict warnings, and comprehensive testing.

## Supported Platforms

- **Windows**: MSVC (cl compiler) with Visual Studio 2022
- **Ubuntu**: GCC 13+ and Clang 17+
- **Architectures**: x64 (tested), should work on other architectures

## Features

- ✅ **Step 1**: Character frequency analysis with unit tests (333 'X's, 223,000 't's verified)
- ✅ **Step 2**: Optimal Huffman tree construction using priority queue algorithm
- ✅ **Step 3**: Prefix code table generation via tree traversal
- ✅ **Step 4**: Binary header format with frequency table for reconstruction
- ✅ **Step 5**: Efficient bit-level encoding with padding management
- ✅ **Step 6**: Header parsing and tree reconstruction for decoding
- ✅ **Step 7**: Complete decompression with exact original file reproduction

## Architecture

### Modern C++23 Features
- `std::expected` for error handling (no exceptions)
- `std::variant` for tree node types (Leaf/Branch)  
- `std::string_view` for efficient string handling
- `std::span` for safe array access
- `std::unique_ptr` for memory management
- STL algorithms throughout

### Library Design
- **huffman_lib**: Core compression/decompression library
- **huffc**: Command-line compression tool
- **huffd**: Command-line decompression tool
- Comprehensive GTest suite with edge case coverage

### File Format
Binary format with little-endian encoding:
```
[4B] Magic: "HFN1"
[1B] Flags (reserved)
[8B] Original size
[2B] Frequency entry count  
[9B per entry] Symbol + frequency pairs
[1B] Padding bits (0-7)
[Variable] Compressed payload
```

## Build & Test

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel
ctest -V
```

## Usage

```bash
# Compress
./huffc input.txt compressed.huf

# Decompress  
./huffd compressed.huf restored.txt

# Verify
diff input.txt restored.txt  # Should be identical
```

## Performance

**Les Misérables test results:**
- Original size: 3.3M (3,369,045 bytes)
- Compressed: 1.9M (1,971,084 bytes) 
- **Compression ratio: 1.71:1**
- Decompressed file matches original exactly

## Build Configuration

- **C++23 standard** with strict compliance
- **No exceptions** (`-fno-exceptions`)
- **All warnings as errors** with pedantic flags
- **Endian-safe I/O** for cross-platform compatibility
- **Edge case handling**: empty files, single symbols, corrupt data

## Testing

- Unit tests verify exact character frequencies
- Roundtrip tests ensure perfect reconstruction
- Edge case coverage (empty files, single symbols)
- Memory safety with smart pointers
- No undefined behavior

This implementation demonstrates production-quality C++23 code with modern idioms, comprehensive error handling, and robust testing practices.