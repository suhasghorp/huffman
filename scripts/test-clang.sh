#!/bin/bash
set -euo pipefail

echo "=== Huffman Project Clang++19 Build and Test ==="
echo "Compiler versions:"
clang-19 --version
clang++-19 --version
echo ""

cd /workspace

# Clean any existing builds
echo "=== Cleaning previous builds ==="
rm -rf build-debug build-release
echo ""

# Test Debug build
echo "=== Building Debug configuration ==="
mkdir -p build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang-19 -DCMAKE_CXX_COMPILER=clang++-19 ..
make -j$(nproc)
echo ""

echo "=== Running Debug tests ==="
ctest --output-on-failure --verbose
echo ""

echo "=== Testing CLI tools (Debug) ==="
./huffc ../tests/test.txt test-compressed.huf
./huffd test-compressed.huf test-restored.txt
diff ../tests/test.txt test-restored.txt
echo "✅ CLI tools work correctly in Debug mode"
echo ""

cd ..

# Test Release build
echo "=== Building Release configuration ==="
mkdir -p build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-19 -DCMAKE_CXX_COMPILER=clang++-19 ..
make -j$(nproc)
echo ""

echo "=== Running Release tests ==="
ctest --output-on-failure --verbose
echo ""

echo "=== Testing CLI tools (Release) ==="
./huffc ../tests/test.txt test-compressed.huf
./huffd test-compressed.huf test-restored.txt
diff ../tests/test.txt test-restored.txt
echo "✅ CLI tools work correctly in Release mode"
echo ""

cd ..

echo "🎉 All Clang++19 tests passed successfully!"
echo "   ✅ Debug build and tests"
echo "   ✅ Release build and tests" 
echo "   ✅ CLI tools functionality"
echo ""
echo "The project is ready for GitHub Actions with Clang++19!"