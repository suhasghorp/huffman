cmake_minimum_required(VERSION 3.20)
project(huffman VERSION 1.0.0 LANGUAGES CXX)

# C++23 standard requirement
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable exceptions globally
if(NOT MSVC)
    add_compile_options(-fno-exceptions)
    # Note: MSVC exception handling left enabled as /EHs- conflicts with STL
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

# Set policy for download timestamps to avoid warnings
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
  GIT_SHALLOW    TRUE
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Disable GTest's own tests and examples
set(BUILD_GMOCK OFF CACHE BOOL "")
set(INSTALL_GTEST OFF CACHE BOOL "")

# Temporarily disable warnings as errors for dependencies
set(CMAKE_WARN_AS_ERROR_OLD ${CMAKE_WARN_AS_ERROR})
set(CMAKE_WARN_AS_ERROR OFF)

FetchContent_MakeAvailable(googletest)

# Restore warning settings
set(CMAKE_WARN_AS_ERROR ${CMAKE_WARN_AS_ERROR_OLD})

# Disable warnings for GTest targets
if(TARGET gtest)
    if(MSVC)
        # Remove /W4 and set /w to suppress all warnings
        get_target_property(gtest_options gtest COMPILE_OPTIONS)
        if(gtest_options)
            list(REMOVE_ITEM gtest_options /W4)
        endif()
        target_compile_options(gtest PRIVATE /w)
        target_compile_options(gtest_main PRIVATE /w)
        
        # Also remove /W4 from the targets and set /w
        set_target_properties(gtest PROPERTIES COMPILE_OPTIONS "/w")
        set_target_properties(gtest_main PROPERTIES COMPILE_OPTIONS "/w")
    else()
        target_compile_options(gtest PRIVATE -w)
        target_compile_options(gtest_main PRIVATE -w)
    endif()
endif()

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

add_test(NAME huffman_tests COMMAND $<TARGET_FILE:huffman_tests>)
