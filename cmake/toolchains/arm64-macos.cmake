# ARM64 macOS Cross-Compilation Toolchain (Apple Silicon)
# This toolchain file configures CMake for cross-compiling to ARM64 macOS

set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_SYSTEM_PROCESSOR arm64)

# Set the target architecture
set(CMAKE_OSX_ARCHITECTURES arm64)

# Minimum macOS version for ARM64 (Apple Silicon requires macOS 11.0+)
set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0)

# Use Clang compiler
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# ARM64-specific compiler flags for Apple Silicon
set(CMAKE_C_FLAGS_INIT "-arch arm64 -mcpu=apple-a14")
set(CMAKE_CXX_FLAGS_INIT "-arch arm64 -mcpu=apple-a14")

# Release optimizations for Apple Silicon
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG -arch arm64 -mcpu=apple-a14")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG -arch arm64 -mcpu=apple-a14")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g -arch arm64")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g -arch arm64")

# Linker flags for ARM64 macOS
set(CMAKE_EXE_LINKER_FLAGS_INIT "-arch arm64")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-arch arm64")

# Set Qt6 path for ARM64 macOS (adjust as needed)
# set(Qt6_DIR "/opt/homebrew/lib/cmake/Qt6")

# macOS-specific definitions
add_compile_definitions(
    __APPLE__
    __MACH__
)

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Framework search paths for macOS
set(CMAKE_FRAMEWORK_PATH
    /System/Library/Frameworks
    /Library/Frameworks
    /opt/homebrew/Frameworks
)

# Additional include paths for Homebrew on Apple Silicon
set(CMAKE_PREFIX_PATH
    /opt/homebrew
    /opt/homebrew/lib/cmake
)

# Cross-compilation settings
if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    # Cross-compiling from Intel Mac to Apple Silicon
    set(CMAKE_CROSSCOMPILING TRUE)

    # Search for programs in the build host directories
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    # Search for libraries and headers in the target directories
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
endif()
