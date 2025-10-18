# Huffman Project - Compiler Compatibility Rules

This document contains important compiler-specific fixes and patterns that must be maintained for cross-platform compatibility.

## Compiler-Specific Issues and Solutions

### 1. Circular Dependency with std::unique_ptr (Clang Issue)
**Problem**: Clang is stricter about incomplete types with std::unique_ptr than GCC/MSVC
**Solution**: Use custom deleter pattern
```cpp
// WRONG - causes "invalid application of 'sizeof' to incomplete type"
struct Node;
struct Branch {
    std::unique_ptr<Node> left, right;  // Circular dependency
};

// RIGHT - custom deleter breaks the cycle
struct NodeDeleter {
    void operator()(Node* ptr) const;  // Declared in header, defined in .cpp
};
using NodePtr = std::unique_ptr<Node, NodeDeleter>;
```

### 2. MSVC Warning Flag Conflicts (Windows CI Issue)
**Problem**: MSVC warning D9025 "overriding /W4 with /w"
**Solution**: Properly isolate external dependency warning suppression
```cmake
# WRONG - causes flag conflicts
target_compile_options(gtest PRIVATE /w)  # Conflicts with global /W4

# RIGHT - completely replace warning flags
set_target_properties(gtest PROPERTIES COMPILE_OPTIONS "/w")
```

### 3. MSVC Multi-Config Generator (Windows CI Issue)
**Problem**: CMAKE_BUILD_TYPE ignored at configure time, executables in wrong subdirectory
**Solution**: Specify configuration during build time
```yaml
# WRONG - config ignored
- name: Build
  run: cmake --build build-Release --parallel

# RIGHT - explicit config for MSVC
- name: Build (Windows)  
  run: cmake --build build-Release --config Release --parallel
```

### 4. CTest Configuration Requirement (Windows CI Issue)
**Problem**: "Test not available without configuration" on Windows
**Solution**: Add -C flag for MSVC builds
```yaml
# WRONG - missing config
- name: Test
  run: ctest --output-on-failure --verbose

# RIGHT - config specified for MSVC  
- name: Test (Windows)
  run: ctest -C Release --output-on-failure --verbose
```

### 5. Type Narrowing Warnings (Clang/MSVC Issue)
**Problem**: Implicit int to custom type conversion warnings
**Solution**: Explicit type construction
```cpp
// WRONG - narrowing conversion warning
std::fill(lengths.begin(), lengths.end(), 0);

// RIGHT - explicit type construction  
std::fill(lengths.begin(), lengths.end(), byte{0});
```

### 6. Unused Variable Warnings (Clang Issue)
**Problem**: Clang treats unused constexpr variables as errors with -Werror
**Solution**: Remove unused variables or mark with [[maybe_unused]]
```cpp
// WRONG - unused variable causes error
constexpr std::size_t ENTRY_SIZE = 9;  // Never used

// RIGHT - remove if truly unused
// constexpr std::size_t ENTRY_SIZE = 9;  // Removed

// OR mark as maybe unused if needed for documentation
[[maybe_unused]] constexpr std::size_t ENTRY_SIZE = 9;
```

## Build Matrix Patterns

### CMake Configuration
```cmake
# Always use these patterns for cross-compiler compatibility:

# 1. Explicit target properties over global flags when isolating dependencies
set_target_properties(external_lib PROPERTIES COMPILE_OPTIONS "/w")  # MSVC
set_target_properties(external_lib PROPERTIES COMPILE_OPTIONS "-w")  # GCC/Clang

# 2. Generator expressions for executable paths
add_test(NAME test_name COMMAND $<TARGET_FILE:test_target>)

# 3. Explicit type construction for custom types
CustomType value{raw_value};  # Not: CustomType value = raw_value;
```

### GitHub Actions Patterns
```yaml
# Always separate Windows and Unix build steps:

- name: Build (Windows)
  if: matrix.os == 'windows-latest'
  run: cmake --build build-${{ matrix.build_type }} --config ${{ matrix.build_type }} --parallel

- name: Build (Ubuntu)  
  if: matrix.os == 'ubuntu-latest'
  run: cmake --build build-${{ matrix.build_type }} --parallel

- name: Test (Windows)
  if: matrix.os == 'windows-latest' 
  run: ctest -C ${{ matrix.build_type }} --output-on-failure --verbose

- name: Test (Ubuntu)
  if: matrix.os == 'ubuntu-latest'
  run: ctest --output-on-failure --verbose
```

## Testing Strategy

### Local Validation with Devcontainers
1. **Clang Testing**: Use `.devcontainer/devcontainer.json` with Ubuntu 24.04 + Clang 19
2. **MSVC Testing**: Use `.devcontainer/devcontainer-windows.json` with Windows Server + MSVC
3. **Scripts**: `scripts/test-clang.sh` and `scripts/test-msvc.ps1` for validation

### Pre-Push Checklist
- [ ] Test with Linux Clang devcontainer
- [ ] Test with Windows MSVC devcontainer  
- [ ] Verify all warning levels pass with -Werror/W4
- [ ] Check both Debug and Release configurations
- [ ] Validate CLI tools functionality

## Memory Management Patterns

### Safe Forward Declarations
```cpp
// Pattern for breaking circular dependencies:
struct TypeA;

struct TypeADeleter {
    void operator()(TypeA* ptr) const;  // Define in .cpp where TypeA is complete
};

using TypeAPtr = std::unique_ptr<TypeA, TypeADeleter>;

template<typename... Args>
TypeAPtr make_type_a_ptr(Args&&... args) {
    return TypeAPtr(new TypeA(std::forward<Args>(args)...), TypeADeleter{});
}
```

This pattern ensures compatibility across GCC, Clang, and MSVC compilers while maintaining clean, modern C++23 code.