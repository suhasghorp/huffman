# Devcontainer Development Guide

This project provides three devcontainer configurations for development and testing:

1. **Ubuntu GCC-13** (`.devcontainer/devcontainer.json`) - **Default Development Environment**
2. **Ubuntu Clang++19** (`.devcontainer/devcontainer-clang.json`)
3. **Windows MSVC** (`.devcontainer/devcontainer-windows.json`)

## Default Development Environment

**Ubuntu GCC-13** is the primary development environment, providing:
- Full C++23 support with GCC-13
- Comprehensive toolchain: cmake, ninja-build, gdb, valgrind, clang-tidy
- VS Code integration with CMake Tools, IntelliSense, and debugging
- Consistent `/workspaces/huffman` workspace folder

## Prerequisites

- Docker Desktop with Windows containers support (for Windows MSVC testing)
- VS Code with Dev Containers extension (optional, can use Docker directly)

## Usage Patterns

### Option 1: Using VS Code Dev Containers Extension

#### Default Development (GCC-13)
1. Open the project in VS Code
2. Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac)
3. Select "Dev Containers: Reopen in Container"
4. This will use the default GCC-13 environment
5. Run `scripts/test-gcc.sh` to build and test

#### Alternative Environments
To use a specific devcontainer:
1. Press `Ctrl+Shift+P` → "Dev Containers: Open Folder in Container..."
2. Select "From '.devcontainer' folder" 
3. Choose your desired configuration:
   - `devcontainer.json` - Ubuntu GCC-13 (Default)
   - `devcontainer-clang.json` - Ubuntu Clang++19 testing
   - `devcontainer-windows.json` - Windows MSVC testing

### Option 2: Using Docker Directly

#### Ubuntu GCC-13 Development (Default)
```bash
# Using the default devcontainer
cd /path/to/huffman
# VS Code will build the container automatically, or you can build manually:
docker build -t huffman-gcc13 -f .devcontainer/devcontainer.json .

# Run the test script inside the container
docker run --rm -v "$(pwd):/workspaces/huffman" huffman-gcc13 /workspaces/huffman/scripts/test-gcc.sh
```

#### Ubuntu Clang++19 Testing
```bash
# Build the container
cd /path/to/huffman
docker build -t huffman-clang19 -f .devcontainer/devcontainer-clang.json .

# Run the test script
docker run --rm -v "$(pwd):/workspace" huffman-clang19 /workspace/scripts/test-clang.sh
```

#### Windows MSVC Testing
```powershell
# Build the container (requires Windows containers mode)
cd C:\path\to\huffman
docker build -t huffman-msvc -f .devcontainer/Dockerfile.windows .

# Run the test script
docker run --rm -v "${PWD}:C:\workspace" huffman-msvc powershell -File C:\workspace\scripts\test-msvc.ps1
```

## Test Scripts

### Ubuntu GCC-13 Script (`scripts/test-gcc.sh`) - Default
- Tests Debug and Release configurations
- Validates all unit tests pass
- Tests CLI tools functionality  
- Uses GCC-13 with full C++23 support
- **Must be run inside the GCC-13 devcontainer**

### Ubuntu Clang Script (`scripts/test-clang.sh`)
- Tests Debug and Release configurations
- Validates all unit tests pass
- Tests CLI tools functionality
- Uses Clang 19.1.7 with full C++23 support
- **Must be run inside the Clang devcontainer**

### Windows MSVC Script (`scripts/test-msvc.ps1`)
- Tests Debug and Release configurations with proper MSVC multi-config handling
- Validates CTest with `-C` configuration flags
- Tests CLI tools in Windows environment
- Uses Visual Studio 2022 Build Tools

## Development Workflow

### Primary Development (GCC-13)
1. **Use VS Code with default devcontainer** (Ubuntu GCC-13)
2. **Build and test regularly**:
   ```bash
   # Inside the devcontainer terminal
   ./scripts/test-gcc.sh
   ```
3. **Note**: Do NOT use host-installed g++/clang. Always develop inside the devcontainer.

### Pre-Push Testing Sequence
1. **Test with default GCC-13 devcontainer**:
   ```bash
   # In VS Code, open in devcontainer and run:
   ./scripts/test-gcc.sh
   ```
2. **Test with Clang devcontainer** (optional but recommended):
   ```bash
   # Switch to Clang devcontainer, then:
   ./scripts/test-clang.sh
   ```
3. **Push to GitHub** - CI will test all three compilers (GCC, Clang, MSVC)

### Quick Validation Commands
```bash
# Quick Clang validation (Linux/macOS)
make -C . test-clang

# Quick MSVC validation (Windows with PowerShell)
.\scripts\test-msvc.ps1
```

## Troubleshooting

### Common Issues

#### Docker Windows Container Mode
If you get "image operating system cannot be used on this platform":
- Switch Docker Desktop to Windows containers mode
- Right-click Docker Desktop tray icon → "Switch to Windows containers"

#### Volume Mount Issues (Windows)
If paths don't mount correctly:
- Use absolute paths: `docker run -v "C:\full\path\to\project:C:\workspace"`
- Ensure drive sharing is enabled in Docker Desktop settings

#### CMake Not Found
If CMake isn't available in container:
- Wait for `postCreateCommand` to complete during first build
- Rebuild container: `docker build --no-cache`

#### Wrong Compiler Detected (GCC Development)
If CMake Tools picks up host compiler instead of container GCC-13:
1. Open Command Palette: `Ctrl+Shift+P`
2. Run "CMake: Select Kit"
3. Choose "[Unspecified]" or "GCC 13.x.x" 
4. Verify in CMake output that it shows `/usr/bin/g++-13`
5. If still wrong, delete `build-debug` and `build-release` folders and reconfigure

#### IntelliSense Not Working
If C++ IntelliSense isn't working properly:
1. Check that `ms-vscode.cpptools` extension is installed
2. Verify C++ standard is set to C++23 in workspace settings
3. Run "C/C++: Rescan Workspace" from Command Palette
4. Check `.vscode/c_cpp_properties.json` has correct compiler path

### Container Resource Requirements

#### Linux Clang Container
- **RAM**: 2GB minimum, 4GB recommended
- **Disk**: 2GB for container + dependencies
- **CPU**: Any x64 architecture

#### Windows MSVC Container  
- **RAM**: 8GB minimum, 16GB recommended (Visual Studio Build Tools)
- **Disk**: 10GB for container + VS Build Tools
- **CPU**: x64 required
- **OS**: Windows 10/11 or Windows Server with containers support

## Compiler-Specific Validations

The devcontainers test for the specific issues documented in `WARP.md`:

### Clang-Specific Tests
- ✅ Circular dependency with custom deleter
- ✅ Unused variable warnings with `-Werror`
- ✅ Type narrowing warnings
- ✅ Strict incomplete type handling

### MSVC-Specific Tests  
- ✅ Multi-config generator (executables in Debug/Release subdirs)
- ✅ CTest with `-C` configuration flag
- ✅ Warning flag conflicts (`/W4` vs `/w`)
- ✅ Windows path handling in CLI tools

## Integration with CI

The devcontainers mirror the exact GitHub Actions environment:
- **GCC container** (default) → `ubuntu-latest` + `gcc-13` 
- **Clang container** → `ubuntu-latest` + `clang-19`
- **MSVC container** → `windows-latest` + VS Build Tools

Successful testing in the default GCC-13 devcontainer ensures your primary development work is CI-ready. The alternate containers validate cross-compiler compatibility.
