#!/bin/bash
set -euo pipefail

echo "=== Huffman Project GCC-13 Build and Test ==="
echo "Compiler versions:"
gcc-13 --version
g++-13 --version
echo ""

# Navigate to repo root robustly
if command -v git &> /dev/null && git rev-parse --show-toplevel &> /dev/null; then
    cd "$(git rev-parse --show-toplevel)"
else
    # Fallback to current directory if not in git repo
    echo "Warning: Not in a git repository, using current directory"
fi

# Clean any existing builds
echo "=== Cleaning previous builds ==="
rm -rf build-debug build-release
echo ""

# Test Debug build
echo "=== Building Debug configuration ==="
mkdir -p build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/gcc-13 -DCMAKE_CXX_COMPILER=/usr/bin/g++-13 ..
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
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/gcc-13 -DCMAKE_CXX_COMPILER=/usr/bin/g++-13 ..
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

echo "🎉 All GCC-13 tests passed successfully!"
echo "   ✅ Debug build and tests"
echo "   ✅ Release build and tests" 
echo "   ✅ CLI tools functionality"
echo ""
echo "The project is ready for GitHub Actions with GCC-13!"