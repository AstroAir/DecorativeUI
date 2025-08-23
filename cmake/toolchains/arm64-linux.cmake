# ARM64 Linux Cross-Compilation Toolchain
# This toolchain file configures CMake for cross-compiling to ARM64 Linux

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Cross-compilation toolchain
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Set the target architecture
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Where to look for the target environment
set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# ARM64-specific compiler flags
set(CMAKE_C_FLAGS_INIT "-march=armv8-a")
set(CMAKE_CXX_FLAGS_INIT "-march=armv8-a")

# Optimization flags for ARM64
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG -mcpu=cortex-a72")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG -mcpu=cortex-a72")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g")

# Set Qt6 path for ARM64 (adjust as needed)
# set(Qt6_DIR "/opt/qt6-arm64/lib/cmake/Qt6")

# Additional linker flags for ARM64
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--as-needed")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-Wl,--as-needed")

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
