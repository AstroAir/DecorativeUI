# iOS Cross-Compilation Toolchain for Qt6
# This toolchain file configures CMake for cross-compiling to iOS

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_SYSTEM_VERSION 14.0)

# Set the target architecture (can be overridden)
if(NOT DEFINED IOS_ARCH)
    set(IOS_ARCH "arm64")
endif()

set(CMAKE_OSX_ARCHITECTURES ${IOS_ARCH})
set(CMAKE_SYSTEM_PROCESSOR ${IOS_ARCH})

# iOS deployment target
set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0)

# Use Xcode toolchain
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# iOS-specific compiler flags
set(CMAKE_C_FLAGS_INIT "-arch ${IOS_ARCH}")
set(CMAKE_CXX_FLAGS_INIT "-arch ${IOS_ARCH}")

# iOS SDK path
execute_process(
    COMMAND xcrun --sdk iphoneos --show-sdk-path
    OUTPUT_VARIABLE IOS_SDK_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(IOS_SDK_PATH)
    set(CMAKE_OSX_SYSROOT ${IOS_SDK_PATH})
    message(STATUS "iOS SDK found at: ${IOS_SDK_PATH}")
else()
    message(FATAL_ERROR "iOS SDK not found")
endif()

# iOS-specific definitions
add_compile_definitions(
    __IPHONE_OS_VERSION_MIN_REQUIRED=140000
    TARGET_OS_IPHONE=1
    TARGET_IPHONE_SIMULATOR=0
)

# iOS-specific compiler flags
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG -arch ${IOS_ARCH}")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG -arch ${IOS_ARCH}")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-O0 -g -arch ${IOS_ARCH}")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g -arch ${IOS_ARCH}")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-arch ${IOS_ARCH}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-arch ${IOS_ARCH}")

# Set Qt6 path for iOS (adjust as needed)
# set(Qt6_DIR "/path/to/qt6-ios/lib/cmake/Qt6")

# Cross-compilation settings
set(CMAKE_CROSSCOMPILING TRUE)

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# iOS framework search paths
set(CMAKE_FRAMEWORK_PATH
    ${CMAKE_OSX_SYSROOT}/System/Library/Frameworks
    ${CMAKE_OSX_SYSROOT}/System/Library/PrivateFrameworks
)

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# iOS-specific build settings
set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "" CACHE STRING "Development Team ID")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer" CACHE STRING "Code Sign Identity")
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${CMAKE_OSX_DEPLOYMENT_TARGET})

# Note: This toolchain requires:
# 1. Xcode with iOS SDK installed
# 2. Qt6 built for iOS
# 3. Proper code signing setup for device deployment
