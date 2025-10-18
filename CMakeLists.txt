cmake_minimum_required(VERSION 3.20)
project(huffman VERSION 1.0.0 LANGUAGES CXX)

# C++23 standard requirement
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable exceptions globally
add_compile_options(-fno-exceptions)

# Strict warning flags - treat warnings as errors
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wformat=2
    -Wundef
    -Wduplicated-cond
    -Wlogical-op
    -Wuseless-cast
    -Wzero-as-null-pointer-constant
    -Wold-style-cast
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    -Wnull-dereference
)

# Create directories for build artifacts
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tmp)

# Library sources
set(HUFFMAN_LIB_SOURCES
    src/io.cpp
    src/endian.cpp
    src/frequency.cpp
    src/huffman_tree.cpp
    src/code_table.cpp
    src/bitstream.cpp
    src/header.cpp
    src/encode.cpp
    src/decode.cpp
)

# Main library target
add_library(huffman_lib ${HUFFMAN_LIB_SOURCES})
target_include_directories(huffman_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(huffman_lib PUBLIC cxx_std_23)

# Command-line tools
add_executable(huffc src/cli/huffc.cpp)
target_link_libraries(huffc PRIVATE huffman_lib)
target_compile_features(huffc PRIVATE cxx_std_23)

add_executable(huffd src/cli/huffd.cpp)
target_link_libraries(huffd PRIVATE huffman_lib)
target_compile_features(huffd PRIVATE cxx_std_23)

# Testing setup
find_package(GTest REQUIRED)
enable_testing()

set(TEST_SOURCES
    tests/test_frequency.cpp
    tests/test_roundtrip.cpp
)

add_executable(huffman_tests ${TEST_SOURCES})
target_link_libraries(huffman_tests PRIVATE huffman_lib GTest::gtest_main)
target_compile_features(huffman_tests PRIVATE cxx_std_23)
target_compile_definitions(huffman_tests PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_compile_definitions(huffman_tests PRIVATE CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}")

add_test(NAME huffman_tests COMMAND huffman_tests)