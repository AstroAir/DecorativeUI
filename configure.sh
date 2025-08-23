#!/bin/bash

# Build Script for DeclarativeUI with Multi-Platform Support
# This script provides configurable build options and platform detection

set -e

# Default values
BUILD_TYPE="Release"
PRESET="default"
CLEAN_BUILD=false
VERBOSE=false
RUN_TESTS=false
RUN_EXAMPLES=false
ENABLE_LTO=false
ENABLE_UNITY_BUILD=false
ENABLE_CCACHE=true
ENABLE_PCH=true
TARGET_ARCH="x64"

# Function to show help
show_help() {
    cat << EOF
Build Script for DeclarativeUI

Usage: $0 [options]

Options:
  --preset PRESET     CMake preset to use (default: default)
  --arch ARCH         Target architecture: x64, arm64 (default: x64)
  --clean             Clean build directory before building
  --verbose           Enable verbose build output
  --test              Run tests after building
  --examples          Run examples after building
  --lto               Enable Link Time Optimization
  --unity             Enable Unity builds for faster compilation
  --no-ccache         Disable ccache
  --no-pch            Disable precompiled headers
  -h, --help          Show this help message

Available presets:
  default             Standard release build
  debug               Debug build with diagnostics
  performance         High-performance build with LTO
  fast-build          Fast development build
  arm64-linux         ARM64 Linux build
  arm64-macos         ARM64 macOS build (Apple Silicon)

Examples:
  $0 --preset performance --lto --unity
  $0 --arch arm64 --preset arm64-linux
  $0 --clean --test --verbose
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        --preset)
            PRESET="$2"
            shift 2
            ;;
        --arch)
            TARGET_ARCH="$2"
            shift 2
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --test)
            RUN_TESTS=true
            shift
            ;;
        --examples)
            RUN_EXAMPLES=true
            shift
            ;;
        --lto)
            ENABLE_LTO=true
            shift
            ;;
        --unity)
            ENABLE_UNITY_BUILD=true
            shift
            ;;
        --no-ccache)
            ENABLE_CCACHE=false
            shift
            ;;
        --no-pch)
            ENABLE_PCH=false
            shift
            ;;
        *)
            echo "Unknown argument: $1"
            show_help
            exit 1
            ;;
    esac
done

echo "========================================"
echo "DeclarativeUI Build Script"
echo "========================================"
echo "Configuration:"
echo "  Preset: $PRESET"
echo "  Architecture: $TARGET_ARCH"
echo "  Clean Build: $CLEAN_BUILD"
echo "  Verbose: $VERBOSE"
echo "  Run Tests: $RUN_TESTS"
echo "  Run Examples: $RUN_EXAMPLES"
echo "  LTO: $ENABLE_LTO"
echo "  Unity Build: $ENABLE_UNITY_BUILD"
echo "  ccache: $ENABLE_CCACHE"
echo "  PCH: $ENABLE_PCH"
echo

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found in PATH"
    echo "Please install CMake and add it to your PATH"
    exit 1
fi

# Check for Qt6
echo "Checking for Qt6..."
if ! command -v qmake &> /dev/null; then
    echo "WARNING: qmake not found in PATH"
    echo "Make sure Qt6 is installed and in your PATH"
fi

# Detect platform and architecture
echo "Detecting platform and architecture..."
DETECTED_OS=$(uname -s)
DETECTED_ARCH=$(uname -m)

case $DETECTED_ARCH in
    x86_64|amd64)
        DETECTED_ARCH="x64"
        ;;
    aarch64|arm64)
        DETECTED_ARCH="arm64"
        ;;
    armv7l)
        DETECTED_ARCH="arm"
        ;;
esac

echo "Detected OS: $DETECTED_OS"
echo "Detected architecture: $DETECTED_ARCH"

# Clean build if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning previous builds..."
    rm -rf build build-*
fi

# Set additional CMake variables based on options
CMAKE_EXTRA_VARS=""
if [ "$ENABLE_LTO" = true ]; then
    CMAKE_EXTRA_VARS="$CMAKE_EXTRA_VARS -DENABLE_LTO=ON"
fi
if [ "$ENABLE_UNITY_BUILD" = true ]; then
    CMAKE_EXTRA_VARS="$CMAKE_EXTRA_VARS -DENABLE_UNITY_BUILD=ON"
fi
if [ "$ENABLE_CCACHE" = true ]; then
    CMAKE_EXTRA_VARS="$CMAKE_EXTRA_VARS -DENABLE_CCACHE=ON"
else
    CMAKE_EXTRA_VARS="$CMAKE_EXTRA_VARS -DENABLE_CCACHE=OFF"
fi
if [ "$ENABLE_PCH" = true ]; then
    CMAKE_EXTRA_VARS="$CMAKE_EXTRA_VARS -DENABLE_PRECOMPILED_HEADERS=ON"
else
    CMAKE_EXTRA_VARS="$CMAKE_EXTRA_VARS -DENABLE_PRECOMPILED_HEADERS=OFF"
fi

# Configure build
echo
echo "Configuring build with preset: $PRESET"
if [ -z "$CMAKE_EXTRA_VARS" ]; then
    cmake --preset "$PRESET"
else
    cmake --preset "$PRESET" $CMAKE_EXTRA_VARS
fi

# Build
echo
echo "Building..."
if [ "$VERBOSE" = true ]; then
    cmake --build --preset "$PRESET" --verbose
else
    cmake --build --preset "$PRESET"
fi

# Run tests if requested
if [ "$RUN_TESTS" = true ]; then
    echo
    echo "Running tests..."
    cd build* 2>/dev/null || cd build
    ctest --output-on-failure --parallel $(nproc)
    if [ $? -eq 0 ]; then
        echo "All tests passed!"
    else
        echo "WARNING: Some tests failed"
    fi
    cd ..
fi

# Run examples if requested
if [ "$RUN_EXAMPLES" = true ]; then
    echo
    echo "Running examples..."
    cd build* 2>/dev/null || cd build
    if [ -f "examples/basic/HelloWorldExample" ]; then
        echo "Running HelloWorldExample..."
        ./examples/basic/HelloWorldExample
    fi
    cd ..
fi

echo
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
