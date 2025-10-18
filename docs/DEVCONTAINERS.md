# Devcontainer Testing Guide

This project provides two devcontainer configurations for local testing before pushing to GitHub Actions:

1. **Linux Clang++19** (`.devcontainer/devcontainer.json`)
2. **Windows MSVC** (`.devcontainer/devcontainer-windows.json`)

## Prerequisites

- Docker Desktop with Windows containers support (for Windows MSVC testing)
- VS Code with Dev Containers extension (optional, can use Docker directly)

## Usage Patterns

### Option 1: Using VS Code Dev Containers Extension

1. Open the project in VS Code
2. Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac)
3. Select "Dev Containers: Reopen in Container"
4. Choose the desired configuration:
   - `devcontainer.json` for Clang++19 testing
   - `devcontainer-windows.json` for MSVC testing

### Option 2: Using Docker Directly

#### Linux Clang++19 Testing
```bash
# Build the container
cd /path/to/huffman
docker build -t huffman-clang19 -f .devcontainer/Dockerfile.clang .

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

### Linux Clang Script (`scripts/test-clang.sh`)
- Tests Debug and Release configurations
- Validates all unit tests pass
- Tests CLI tools functionality
- Uses Clang 19.1.7 with full C++23 support

### Windows MSVC Script (`scripts/test-msvc.ps1`)
- Tests Debug and Release configurations with proper MSVC multi-config handling
- Validates CTest with `-C` configuration flags
- Tests CLI tools in Windows environment
- Uses Visual Studio 2022 Build Tools

## Pre-Push Workflow

### Recommended Testing Sequence
1. **Test locally with GCC** (your current environment)
2. **Test with Clang devcontainer**:
   ```bash
   docker run --rm -v "$(pwd):/workspace" huffman-clang19 /workspace/scripts/test-clang.sh
   ```
3. **Test with MSVC devcontainer** (if Windows Docker available):
   ```powershell
   docker run --rm -v "${PWD}:C:\workspace" huffman-msvc powershell -File C:\workspace\scripts\test-msvc.ps1
   ```
4. **Push to GitHub** with confidence

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

These devcontainers mirror the exact GitHub Actions environment:
- **Clang container** → `ubuntu-latest` + `clang-19`
- **MSVC container** → `windows-latest` + VS Build Tools

Successful local testing virtually guarantees CI will pass!