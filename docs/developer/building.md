# Building DeclarativeUI

This guide covers building DeclarativeUI from source for development and distribution.

## Prerequisites

### Required Dependencies

- **Qt6** (6.2 or later) with development packages
  - Qt6::Core - Core Qt functionality
  - Qt6::Widgets - Widget system
  - Qt6::Network - Network operations
  - Qt6::Test - Unit testing framework
- **CMake** (3.20 or higher) - Build system
- **C++20 compatible compiler**:
  - GCC 10+ (Linux)
  - Clang 12+ (macOS)
  - MSVC 2019+ (Windows)

### Optional Dependencies

- **Ninja** - Fast build system (recommended)
- **Doxygen** - API documentation generation
- **clang-format** - Code formatting
- **clang-tidy** - Static analysis
- **Valgrind** - Memory leak detection (Linux)

## Quick Build

### Windows

```bat
# Clone repository
git clone https://github.com/DeclarativeUI/DeclarativeUI.git
cd DeclarativeUI

# Use provided build script
build.bat

# Or build with specific options
build.bat --debug --verbose
```

### Linux/macOS

```bash
# Clone repository
git clone https://github.com/DeclarativeUI/DeclarativeUI.git
cd DeclarativeUI

# Configure and build
cmake --preset default
cmake --build --preset default

# Run tests
cd build && ctest --output-on-failure
```

## Build Configuration

### CMake Presets

The project includes several CMake presets for different use cases:

| Preset | Description | Use Case |
|--------|-------------|----------|
| `default` | Release build with all features | Production builds |
| `debug` | Debug build with diagnostics | Development |
| `release` | Optimized release build | Distribution |
| `minimal` | Library only, no examples/tests | Integration |
| `command-dev` | Command system development | Command development |
| `legacy-only` | Components only, no Commands | Legacy integration |

### Build Options

Configure the build with CMake options:

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_EXAMPLES` | ON | Build example applications |
| `BUILD_TESTS` | ON | Build test suite |
| `BUILD_SHARED_LIBS` | OFF | Build shared libraries |
| `BUILD_COMMAND_SYSTEM` | ON | Enable Command system |
| `BUILD_ADAPTERS` | ON | Enable integration adapters |
| `ENABLE_COMMAND_DEBUG` | OFF | Enable Command debug output |

### Manual Configuration

```bash
# Custom configuration
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_EXAMPLES=ON \
  -DBUILD_TESTS=ON \
  -DBUILD_COMMAND_SYSTEM=ON \
  -DBUILD_ADAPTERS=ON \
  -DCMAKE_INSTALL_PREFIX=/usr/local
```

## Development Builds

### Debug Build

```bash
# Full debug build with all diagnostics
cmake --preset debug
cmake --build --preset debug

# Or manually
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDECLARATIVE_UI_DEBUG=ON \
  -DENABLE_COMMAND_DEBUG=ON
```

### Development with Hot Reload

```bash
# Build with hot reload enabled
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON
cmake --build .

# Run example with hot reload
./build/examples/basic/BasicHotReloadExample
```

## Platform-Specific Instructions

### Windows (MSVC)

```bat
# Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# Configure with Visual Studio generator
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Or use MSBuild directly
msbuild DeclarativeUI.sln /p:Configuration=Release
```

### Windows (MinGW)

```bash
# Configure with MinGW
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .
```

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  ninja-build \
  qt6-base-dev \
  qt6-tools-dev \
  libqt6core6 \
  libqt6widgets6 \
  libqt6network6 \
  libqt6test6

# Build
cmake --preset default
cmake --build --preset default
```

### Linux (Fedora/CentOS)

```bash
# Install dependencies
sudo dnf install -y \
  gcc-c++ \
  cmake \
  ninja-build \
  qt6-qtbase-devel \
  qt6-qttools-devel

# Build
cmake --preset default
cmake --build --preset default
```

### macOS

```bash
# Install dependencies via Homebrew
brew install qt6 cmake ninja

# Set Qt path
export CMAKE_PREFIX_PATH="$(brew --prefix qt6)"

# Build
cmake --preset default
cmake --build --preset default
```

## Testing

### Running Tests

```bash
# Run all tests
ctest --config Release --output-on-failure

# Run specific test categories
ctest -R "unit" --output-on-failure
ctest -R "command" --output-on-failure
ctest -R "integration" --output-on-failure

# Run with parallel execution
ctest -j 4 --output-on-failure
```

### Test Categories

- **Unit Tests**: Test individual components in isolation
- **Integration Tests**: Test component interactions
- **Command Tests**: Test the Command system
- **Performance Tests**: Benchmark critical operations

### Memory Testing

```bash
# Linux: Run with Valgrind
valgrind --tool=memcheck --leak-check=full ./build/tests/unit/test_core

# Windows: Use Application Verifier or similar tools
# macOS: Use Instruments or AddressSanitizer
```

## Installation

### System Installation

```bash
# Install to system directories
cmake --build . --target install

# Custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/declarativeui
cmake --build . --target install
```

### Package Creation

```bash
# Create distribution packages
cmake --build . --target package

# Create source package
cmake --build . --target package_source
```

## Troubleshooting

### Common Issues

#### Qt6 Not Found

```bash
# Set Qt installation path
export CMAKE_PREFIX_PATH=/path/to/qt6
# Windows: set CMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2019_64

# Or specify Qt directory directly
cmake .. -DQt6_DIR=/path/to/qt6/lib/cmake/Qt6
```

#### Compiler Issues

```bash
# Check compiler version
gcc --version    # Should be 10+
clang --version  # Should be 12+

# Specify compiler explicitly
cmake .. -DCMAKE_CXX_COMPILER=g++-11
```

#### Build Errors

```bash
# Clean build
rm -rf build
mkdir build && cd build

# Verbose build output
cmake --build . --verbose

# Check CMake configuration
cmake .. --debug-output
```

### Performance Issues

#### Slow Builds

```bash
# Use Ninja for faster builds
cmake .. -G Ninja

# Parallel builds
cmake --build . --parallel 8

# Disable examples/tests for faster builds
cmake .. -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF
```

#### Large Memory Usage

```bash
# Reduce parallel compilation
cmake --build . --parallel 2

# Use shared libraries to reduce link time
cmake .. -DBUILD_SHARED_LIBS=ON
```

## Continuous Integration

### GitHub Actions

The project includes GitHub Actions workflows for:

- **CI**: Multi-platform testing and building
- **Code Quality**: Static analysis and formatting checks
- **Documentation**: Automated documentation deployment
- **Releases**: Automated release creation and asset upload

### Local CI Testing

```bash
# Run the same checks as CI locally
cmake --preset default
cmake --build --preset default
ctest --output-on-failure

# Code formatting check
find src/ -name "*.cpp" -o -name "*.hpp" | xargs clang-format --dry-run --Werror

# Static analysis
clang-tidy -p build src/**/*.cpp
```

## Advanced Build Options

### Custom Qt Installation

```bash
# Use specific Qt installation
cmake .. -DCMAKE_PREFIX_PATH="/opt/Qt/6.5.0/gcc_64"

# Multiple Qt installations
cmake .. -DQt6_DIR="/opt/Qt/6.5.0/gcc_64/lib/cmake/Qt6"
```

### Cross-Compilation

```bash
# Example: Cross-compile for ARM64
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=toolchain-arm64.cmake \
  -DCMAKE_BUILD_TYPE=Release
```

### Static Linking

```bash
# Build with static Qt (if available)
cmake .. \
  -DBUILD_SHARED_LIBS=OFF \
  -DQt6_USE_STATIC_LIBS=ON
```

## Build Optimization

### Compiler Optimizations

```bash
# Release with debug info
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Maximum optimization
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG -march=native"
```

### Link Time Optimization

```bash
# Enable LTO for smaller binaries
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

## Next Steps

- **[Contributing Guide](contributing.md)** - Learn how to contribute
- **[Testing Guide](testing.md)** - Understand our testing practices
- **[Architecture](architecture.md)** - Learn about the system design
- **[API Reference](../api/core.md)** - Explore the complete API
