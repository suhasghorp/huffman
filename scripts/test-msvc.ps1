#!/usr/bin/env pwsh

# Huffman Project MSVC Build and Test Script
# This script validates the project builds correctly with MSVC before pushing to GitHub

$ErrorActionPreference = "Stop"

Write-Host "=== Huffman Project MSVC Build and Test ===" -ForegroundColor Green
Write-Host "Compiler versions:" -ForegroundColor Yellow

# Initialize Visual Studio environment
& "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"

cl.exe 2>&1 | Select-String "Microsoft.*Compiler" | Write-Host
Write-Host ""

Set-Location C:\workspace

# Clean any existing builds
Write-Host "=== Cleaning previous builds ===" -ForegroundColor Yellow
Remove-Item -Path "build-debug", "build-release" -Recurse -Force -ErrorAction SilentlyContinue
Write-Host ""

# Test Debug build
Write-Host "=== Building Debug configuration ===" -ForegroundColor Yellow
New-Item -ItemType Directory -Path "build-debug" -Force | Out-Null
Set-Location "build-debug"
cmake -DCMAKE_BUILD_TYPE=Debug ..
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ CMake configuration failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}

cmake --build . --config Debug --parallel
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ Debug build failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}
Write-Host ""

Write-Host "=== Running Debug tests ===" -ForegroundColor Yellow
ctest -C Debug --output-on-failure --verbose
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ Debug tests failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}
Write-Host ""

Write-Host "=== Testing CLI tools (Debug) ===" -ForegroundColor Yellow
& ".\Debug\huffc.exe" "..\tests\test.txt" "test-compressed.huf"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ huffc failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}

& ".\Debug\huffd.exe" "test-compressed.huf" "test-restored.txt" 
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ huffd failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}

fc.exe /b "..\tests\test.txt" "test-restored.txt" | Out-Null
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ File comparison failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}

Write-Host "✅ CLI tools work correctly in Debug mode" -ForegroundColor Green
Write-Host ""

Set-Location ".."

# Test Release build
Write-Host "=== Building Release configuration ===" -ForegroundColor Yellow
New-Item -ItemType Directory -Path "build-release" -Force | Out-Null
Set-Location "build-release"
cmake -DCMAKE_BUILD_TYPE=Release ..
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ CMake configuration failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}

cmake --build . --config Release --parallel
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ Release build failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}
Write-Host ""

Write-Host "=== Running Release tests ===" -ForegroundColor Yellow
ctest -C Release --output-on-failure --verbose
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ Release tests failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}
Write-Host ""

Write-Host "=== Testing CLI tools (Release) ===" -ForegroundColor Yellow
& ".\Release\huffc.exe" "..\tests\test.txt" "test-compressed.huf"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ huffc failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}

& ".\Release\huffd.exe" "test-compressed.huf" "test-restored.txt"
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ huffd failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}

fc.exe /b "..\tests\test.txt" "test-restored.txt" | Out-Null
if ($LASTEXITCODE -ne 0) { 
    Write-Host "❌ File comparison failed" -ForegroundColor Red
    exit $LASTEXITCODE 
}

Write-Host "✅ CLI tools work correctly in Release mode" -ForegroundColor Green
Write-Host ""

Set-Location ".."

Write-Host "🎉 All MSVC tests passed successfully!" -ForegroundColor Green
Write-Host "   ✅ Debug build and tests" -ForegroundColor Green
Write-Host "   ✅ Release build and tests" -ForegroundColor Green
Write-Host "   ✅ CLI tools functionality" -ForegroundColor Green
Write-Host ""
Write-Host "The project is ready for GitHub Actions with MSVC!" -ForegroundColor Green