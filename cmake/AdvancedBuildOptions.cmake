# Advanced Build Options for DeclarativeUI
# This module provides advanced optimization and build configuration options

# Profile-Guided Optimization (PGO) support
option(ENABLE_PGO "Enable Profile-Guided Optimization" OFF)
option(PGO_GENERATE "Generate PGO profile data" OFF)
option(PGO_USE "Use existing PGO profile data" OFF)

# Static linking options
option(ENABLE_STATIC_LINKING "Enable static linking" OFF)
option(ENABLE_STATIC_QT "Link Qt statically" OFF)

# Security hardening options
option(ENABLE_SECURITY_HARDENING "Enable security hardening flags" ON)

# Sanitizer options
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_MSAN "Enable MemorySanitizer" OFF)

# Code coverage options
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Function to configure Profile-Guided Optimization
function(configure_pgo target)
    if(NOT ENABLE_PGO)
        return()
    endif()

    if(MSVC)
        if(PGO_GENERATE)
            target_compile_options(${target} PRIVATE /GL)
            target_link_options(${target} PRIVATE /LTCG:PGI)
            message(STATUS "PGO profile generation enabled for ${target}")
        elseif(PGO_USE)
            target_compile_options(${target} PRIVATE /GL)
            target_link_options(${target} PRIVATE /LTCG:PGO)
            message(STATUS "PGO profile usage enabled for ${target}")
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if(PGO_GENERATE)
            target_compile_options(${target} PRIVATE -fprofile-generate)
            target_link_options(${target} PRIVATE -fprofile-generate)
            message(STATUS "PGO profile generation enabled for ${target}")
        elseif(PGO_USE)
            target_compile_options(${target} PRIVATE -fprofile-use)
            target_link_options(${target} PRIVATE -fprofile-use)
            message(STATUS "PGO profile usage enabled for ${target}")
        endif()
    endif()
endfunction()

# Function to configure static linking
function(configure_static_linking target)
    if(ENABLE_STATIC_LINKING)
        set_target_properties(${target} PROPERTIES
            LINK_SEARCH_START_STATIC ON
            LINK_SEARCH_END_STATIC ON
        )

        if(WIN32 AND MSVC)
            target_compile_options(${target} PRIVATE /MT$<$<CONFIG:Debug>:d>)
        elseif(UNIX)
            target_link_options(${target} PRIVATE -static-libgcc -static-libstdc++)
        endif()

        message(STATUS "Static linking enabled for ${target}")
    endif()

    if(ENABLE_STATIC_QT)
        set_target_properties(${target} PROPERTIES
            QT_USE_STATIC_LIBS ON
        )
        message(STATUS "Static Qt linking enabled for ${target}")
    endif()
endfunction()

# Function to configure security hardening
function(configure_security_hardening target)
    if(NOT ENABLE_SECURITY_HARDENING)
        return()
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /GS         # Buffer security check
            /guard:cf   # Control Flow Guard
        )
        target_link_options(${target} PRIVATE
            /GUARD:CF
            /DYNAMICBASE
            /NXCOMPAT
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -fstack-protector-strong    # Stack protection
            -D_FORTIFY_SOURCE=2         # Fortify source
            -fPIE                       # Position independent executable
        )

        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${target} PRIVATE
                -fcf-protection=full    # Control Flow Integrity
            )
        endif()

        target_link_options(${target} PRIVATE
            -Wl,-z,relro,-z,now        # Full RELRO
            -pie                        # Position independent executable
        )
    endif()

    message(STATUS "Security hardening enabled for ${target}")
endfunction()

# Function to configure sanitizers
function(configure_sanitizers target)
    set(sanitizer_flags "")
    set(sanitizer_libs "")

    if(ENABLE_ASAN)
        list(APPEND sanitizer_flags "-fsanitize=address")
        list(APPEND sanitizer_libs "asan")
        message(STATUS "AddressSanitizer enabled for ${target}")
    endif()

    if(ENABLE_TSAN)
        list(APPEND sanitizer_flags "-fsanitize=thread")
        list(APPEND sanitizer_libs "tsan")
        message(STATUS "ThreadSanitizer enabled for ${target}")
    endif()

    if(ENABLE_UBSAN)
        list(APPEND sanitizer_flags "-fsanitize=undefined")
        list(APPEND sanitizer_libs "ubsan")
        message(STATUS "UndefinedBehaviorSanitizer enabled for ${target}")
    endif()

    if(ENABLE_MSAN)
        list(APPEND sanitizer_flags "-fsanitize=memory")
        list(APPEND sanitizer_libs "msan")
        message(STATUS "MemorySanitizer enabled for ${target}")
    endif()

    if(sanitizer_flags)
        target_compile_options(${target} PRIVATE ${sanitizer_flags})
        target_link_options(${target} PRIVATE ${sanitizer_flags})

        # Add debug info for better stack traces
        target_compile_options(${target} PRIVATE -g -fno-omit-frame-pointer)
    endif()
endfunction()

# Function to configure code coverage
function(configure_coverage target)
    if(NOT ENABLE_COVERAGE)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            --coverage
            -fprofile-arcs
            -ftest-coverage
        )
        target_link_options(${target} PRIVATE --coverage)

        # Find gcov or llvm-cov
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            find_program(GCOV_PATH gcov)
            if(GCOV_PATH)
                message(STATUS "Code coverage enabled with gcov for ${target}")
            endif()
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            find_program(LLVM_COV_PATH llvm-cov)
            if(LLVM_COV_PATH)
                message(STATUS "Code coverage enabled with llvm-cov for ${target}")
            endif()
        endif()
    endif()
endfunction()

# Function to apply all advanced build options
function(configure_advanced_build_options target)
    configure_pgo(${target})
    configure_static_linking(${target})
    configure_security_hardening(${target})
    configure_sanitizers(${target})
    configure_coverage(${target})
endfunction()

# Custom target for PGO workflow
if(ENABLE_PGO)
    add_custom_target(pgo-generate
        COMMENT "Generate PGO profile data"
        COMMAND ${CMAKE_COMMAND} -DPGO_GENERATE=ON -DPGO_USE=OFF ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    )

    add_custom_target(pgo-use
        COMMENT "Build with PGO profile data"
        COMMAND ${CMAKE_COMMAND} -DPGO_GENERATE=OFF -DPGO_USE=ON ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
    )
endif()

# Custom target for coverage report
if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMENT "Generate coverage report"
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND ${LCOV_PATH} --list coverage.info
            COMMAND ${GENHTML_PATH} -o coverage coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()
endif()
