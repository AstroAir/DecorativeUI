# Android Cross-Compilation Toolchain for Qt6
# This toolchain file configures CMake for cross-compiling to Android

set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 21)  # Android API level 21 (Android 5.0)

# Set the target architecture (can be overridden)
if(NOT DEFINED ANDROID_ABI)
    set(ANDROID_ABI "arm64-v8a")
endif()

# Map Android ABI to CMake processor
if(ANDROID_ABI STREQUAL "arm64-v8a")
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(CMAKE_SYSTEM_PROCESSOR armv7-a)
elseif(ANDROID_ABI STREQUAL "x86_64")
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
elseif(ANDROID_ABI STREQUAL "x86")
    set(CMAKE_SYSTEM_PROCESSOR i686)
endif()

# Android NDK path (must be set)
if(NOT DEFINED ANDROID_NDK)
    if(DEFINED ENV{ANDROID_NDK_ROOT})
        set(ANDROID_NDK $ENV{ANDROID_NDK_ROOT})
    elseif(DEFINED ENV{ANDROID_NDK})
        set(ANDROID_NDK $ENV{ANDROID_NDK})
    else()
        message(FATAL_ERROR "ANDROID_NDK must be set to the Android NDK path")
    endif()
endif()

# Android platform (API level)
if(NOT DEFINED ANDROID_PLATFORM)
    set(ANDROID_PLATFORM android-21)
endif()

# Use Android NDK toolchain
set(CMAKE_ANDROID_NDK ${ANDROID_NDK})
set(CMAKE_ANDROID_API ${CMAKE_SYSTEM_VERSION})
set(CMAKE_ANDROID_ARCH_ABI ${ANDROID_ABI})

# Android-specific compiler settings
set(CMAKE_ANDROID_STL_TYPE c++_shared)

# Find the Android toolchain
set(ANDROID_TOOLCHAIN_ROOT ${ANDROID_NDK}/toolchains/llvm/prebuilt)

# Detect host system
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(ANDROID_HOST_TAG linux-x86_64)
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(ANDROID_HOST_TAG darwin-x86_64)
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(ANDROID_HOST_TAG windows-x86_64)
endif()

set(ANDROID_TOOLCHAIN_PREFIX ${ANDROID_TOOLCHAIN_ROOT}/${ANDROID_HOST_TAG})

# Set compilers
if(ANDROID_ABI STREQUAL "arm64-v8a")
    set(ANDROID_TOOLCHAIN_NAME aarch64-linux-android)
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(ANDROID_TOOLCHAIN_NAME armv7a-linux-androideabi)
elseif(ANDROID_ABI STREQUAL "x86_64")
    set(ANDROID_TOOLCHAIN_NAME x86_64-linux-android)
elseif(ANDROID_ABI STREQUAL "x86")
    set(ANDROID_TOOLCHAIN_NAME i686-linux-android)
endif()

set(CMAKE_C_COMPILER ${ANDROID_TOOLCHAIN_PREFIX}/bin/${ANDROID_TOOLCHAIN_NAME}${CMAKE_SYSTEM_VERSION}-clang)
set(CMAKE_CXX_COMPILER ${ANDROID_TOOLCHAIN_PREFIX}/bin/${ANDROID_TOOLCHAIN_NAME}${CMAKE_SYSTEM_VERSION}-clang++)

# Android-specific definitions
add_compile_definitions(
    ANDROID
    __ANDROID_API__=${CMAKE_SYSTEM_VERSION}
)

# Android-specific compiler flags
set(CMAKE_C_FLAGS_INIT "")
set(CMAKE_CXX_FLAGS_INIT "")

# Release optimizations
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g")

# Architecture-specific optimizations
if(ANDROID_ABI STREQUAL "arm64-v8a")
    set(CMAKE_C_FLAGS_RELEASE_INIT "${CMAKE_C_FLAGS_RELEASE_INIT} -mcpu=cortex-a53")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_RELEASE_INIT} -mcpu=cortex-a53")
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -march=armv7-a -mfloat-abi=softfp -mfpu=neon")
    set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -march=armv7-a -mfloat-abi=softfp -mfpu=neon")
endif()

# Set Qt6 path for Android (adjust as needed)
# set(Qt6_DIR "/path/to/qt6-android/lib/cmake/Qt6")

# Cross-compilation settings
set(CMAKE_CROSSCOMPILING TRUE)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Android library search paths
set(CMAKE_FIND_ROOT_PATH
    ${ANDROID_NDK}/platforms/${ANDROID_PLATFORM}/arch-${CMAKE_ANDROID_ARCH_ABI}
    ${ANDROID_NDK}/sources/cxx-stl/llvm-libc++
)

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Android-specific linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--gc-sections")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-Wl,--gc-sections")

# Note: This toolchain requires:
# 1. Android NDK installed and ANDROID_NDK environment variable set
# 2. Qt6 built for Android
# 3. Java SDK for APK packaging (if building applications)
