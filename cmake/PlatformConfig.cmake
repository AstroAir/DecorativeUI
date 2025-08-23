# Platform-specific configuration for DeclarativeUI
# This module provides platform detection and optimization settings

# Platform detection
if(WIN32)
    set(DECLARATIVE_UI_PLATFORM "Windows")
elseif(APPLE)
    set(DECLARATIVE_UI_PLATFORM "macOS")
elseif(UNIX)
    set(DECLARATIVE_UI_PLATFORM "Linux")
else()
    set(DECLARATIVE_UI_PLATFORM "Unknown")
endif()

# Architecture detection (enhanced)
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64")
    set(DECLARATIVE_UI_ARCH "arm64")
    set(DECLARATIVE_UI_ARCH_ARM64 TRUE)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|x64")
    set(DECLARATIVE_UI_ARCH "x64")
    set(DECLARATIVE_UI_ARCH_X64 TRUE)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686|x86")
    set(DECLARATIVE_UI_ARCH "x86")
    set(DECLARATIVE_UI_ARCH_X86 TRUE)
else()
    set(DECLARATIVE_UI_ARCH "unknown")
endif()

message(STATUS "Platform: ${DECLARATIVE_UI_PLATFORM}")
message(STATUS "Architecture: ${DECLARATIVE_UI_ARCH}")

# Platform-specific compiler optimizations
function(configure_platform_optimizations target)
    if(WIN32)
        configure_windows_optimizations(${target})
    elseif(APPLE)
        configure_macos_optimizations(${target})
    elseif(UNIX)
        configure_linux_optimizations(${target})
    endif()
endfunction()

# Windows-specific optimizations
function(configure_windows_optimizations target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4                 # High warning level
            /permissive-        # Strict conformance
            /Zc:__cplusplus     # Correct __cplusplus macro
        )

        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_options(${target} PRIVATE
                /O2             # Optimize for speed
                /Ob2            # Inline expansion
                /Gy             # Function-level linking
            )
            target_link_options(${target} PRIVATE
                /OPT:REF        # Remove unreferenced functions
                /OPT:ICF        # Identical COMDAT folding
            )
        endif()

        # Architecture-specific optimizations
        if(DECLARATIVE_UI_ARCH_ARM64)
            target_compile_options(${target} PRIVATE /favor:ARM64)
        elseif(DECLARATIVE_UI_ARCH_X64)
            target_compile_options(${target} PRIVATE /favor:AMD64)
        endif()
    endif()

    # Windows-specific definitions
    target_compile_definitions(${target} PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
    )
endfunction()

# macOS-specific optimizations
function(configure_macos_optimizations target)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE
            -O3
            -DNDEBUG
        )

        # Architecture-specific optimizations
        if(DECLARATIVE_UI_ARCH_ARM64)
            target_compile_options(${target} PRIVATE -mcpu=apple-a14)
        elseif(DECLARATIVE_UI_ARCH_X64)
            target_compile_options(${target} PRIVATE -march=native -mtune=native)
        endif()
    endif()

    # macOS-specific definitions
    target_compile_definitions(${target} PRIVATE
        __APPLE__
        __MACH__
    )

    # Set minimum macOS version
    if(DECLARATIVE_UI_ARCH_ARM64)
        set_target_properties(${target} PROPERTIES
            OSX_DEPLOYMENT_TARGET 11.0
        )
    else()
        set_target_properties(${target} PROPERTIES
            OSX_DEPLOYMENT_TARGET 10.15
        )
    endif()
endfunction()

# Linux-specific optimizations
function(configure_linux_optimizations target)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE
            -O3
            -DNDEBUG
        )

        # Architecture-specific optimizations
        if(DECLARATIVE_UI_ARCH_ARM64)
            target_compile_options(${target} PRIVATE -mcpu=cortex-a72)
        elseif(DECLARATIVE_UI_ARCH_X64 AND NOT CMAKE_CROSSCOMPILING)
            target_compile_options(${target} PRIVATE -march=native -mtune=native)
        endif()
    endif()

    # Linux-specific definitions
    target_compile_definitions(${target} PRIVATE
        __linux__
        _GNU_SOURCE
    )

    # Enable position independent code
    set_target_properties(${target} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endfunction()

# Qt6 platform-specific configuration
function(configure_qt6_platform target)
    if(WIN32)
        # Windows-specific Qt modules
        find_package(Qt6 COMPONENTS WinExtras QUIET)
        if(Qt6WinExtras_FOUND)
            target_link_libraries(${target} PRIVATE Qt6::WinExtras)
        endif()
    elseif(APPLE)
        # macOS-specific Qt modules
        find_package(Qt6 COMPONENTS MacExtras QUIET)
        if(Qt6MacExtras_FOUND)
            target_link_libraries(${target} PRIVATE Qt6::MacExtras)
        endif()
    elseif(UNIX)
        # Linux-specific Qt modules
        find_package(Qt6 COMPONENTS DBus QUIET)
        if(Qt6DBus_FOUND)
            target_link_libraries(${target} PRIVATE Qt6::DBus)
        endif()
    endif()
endfunction()

# Performance monitoring and profiling setup
function(configure_performance_monitoring target)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        # Enable performance monitoring in debug builds
        target_compile_definitions(${target} PRIVATE
            DECLARATIVE_UI_PERFORMANCE_MONITORING
        )

        if(UNIX AND NOT APPLE)
            # Linux performance tools
            find_library(PROFILER_LIB profiler)
            if(PROFILER_LIB)
                target_link_libraries(${target} PRIVATE ${PROFILER_LIB})
                target_compile_definitions(${target} PRIVATE HAVE_GPERFTOOLS)
            endif()
        endif()
    endif()
endfunction()

# Export platform information for use in other CMake files
set(DECLARATIVE_UI_PLATFORM ${DECLARATIVE_UI_PLATFORM} PARENT_SCOPE)
set(DECLARATIVE_UI_ARCH ${DECLARATIVE_UI_ARCH} PARENT_SCOPE)
set(DECLARATIVE_UI_ARCH_ARM64 ${DECLARATIVE_UI_ARCH_ARM64} PARENT_SCOPE)
set(DECLARATIVE_UI_ARCH_X64 ${DECLARATIVE_UI_ARCH_X64} PARENT_SCOPE)
set(DECLARATIVE_UI_ARCH_X86 ${DECLARATIVE_UI_ARCH_X86} PARENT_SCOPE)
