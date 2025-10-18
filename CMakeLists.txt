cmake_minimum_required(VERSION 3.20)
project(huffman VERSION 1.0.0 LANGUAGES CXX)

# C++23 standard requirement
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable exceptions globally
if(NOT MSVC)
    add_compile_options(-fno-exceptions)
else()
    # MSVC uses /EHs- to disable exceptions
    add_compile_options(/EHs-)
endif()

# Compiler-specific warning flags
if(MSVC)
    # MSVC warning flags
    add_compile_options(
        /W4          # High warning level
        /WX          # Treat warnings as errors
        /permissive- # Disable non-conforming code
        /wd4996      # Disable deprecated warnings
    )
else()
    # GCC/Clang common warnings
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
        -Wzero-as-null-pointer-constant
        -Wold-style-cast
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wnull-dereference
    )
    
    # GCC-specific warnings
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(
            -Wduplicated-cond
            -Wlogical-op
            -Wuseless-cast
        )
    endif()
endif()

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

# Testing setup - use FetchContent for cross-platform compatibility
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50cb48e4a2e5d82b7615f6e9e85e84.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(TEST_SOURCES
    tests/test_frequency.cpp
    tests/test_roundtrip.cpp
)

add_executable(huffman_tests ${TEST_SOURCES})
target_link_libraries(huffman_tests PRIVATE huffman_lib gtest_main)
target_compile_features(huffman_tests PRIVATE cxx_std_23)
target_compile_definitions(huffman_tests PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_compile_definitions(huffman_tests PRIVATE CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}")

add_test(NAME huffman_tests COMMAND huffman_tests)