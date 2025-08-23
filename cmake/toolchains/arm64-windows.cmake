# ARM64 Windows Cross-Compilation Toolchain
# This toolchain file configures CMake for cross-compiling to ARM64 Windows

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR ARM64)

# Use MSVC for ARM64 Windows compilation
set(CMAKE_GENERATOR_PLATFORM ARM64)

# Set the target architecture
set(CMAKE_VS_PLATFORM_NAME ARM64)

# MSVC ARM64 compiler flags
if(MSVC)
    # ARM64-specific optimizations
    set(CMAKE_C_FLAGS_INIT "/favor:ARM64")
    set(CMAKE_CXX_FLAGS_INIT "/favor:ARM64")

    # Release optimizations
    set(CMAKE_C_FLAGS_RELEASE_INIT "/O2 /Ob2 /DNDEBUG /favor:ARM64")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "/O2 /Ob2 /DNDEBUG /favor:ARM64")

    # Debug flags
    set(CMAKE_C_FLAGS_DEBUG_INIT "/Od /RTC1 /Zi")
    set(CMAKE_CXX_FLAGS_DEBUG_INIT "/Od /RTC1 /Zi")

    # Enable parallel compilation
    add_compile_options(/MP)
endif()

# Alternative: MinGW-w64 for ARM64 (if available)
# set(CMAKE_C_COMPILER aarch64-w64-mingw32-gcc)
# set(CMAKE_CXX_COMPILER aarch64-w64-mingw32-g++)

# Where to look for the target environment (for MinGW)
# set(CMAKE_FIND_ROOT_PATH /usr/aarch64-w64-mingw32)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set Qt6 path for ARM64 Windows (adjust as needed)
# set(Qt6_DIR "C:/Qt/6.5.0/msvc2019_arm64/lib/cmake/Qt6")

# Windows-specific definitions
add_compile_definitions(
    WIN32
    _WIN32
    _WINDOWS
    UNICODE
    _UNICODE
)

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
