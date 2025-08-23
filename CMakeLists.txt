cmake_minimum_required(VERSION 3.20)
project(DeclarativeUI VERSION 1.0.0 LANGUAGES CXX)

# **Modern C++ standard**
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# **Include platform configuration and advanced build options**
include(cmake/PlatformConfig.cmake)
include(cmake/AdvancedBuildOptions.cmake)

# **Build Performance Optimizations**
# Enable parallel compilation
if(MSVC)
    # Use all available cores for MSVC
    add_compile_options(/MP)
    # Enable faster PDB generation
    add_compile_options(/Zi)
    add_link_options(/DEBUG:FASTLINK)
else()
    # Enable parallel compilation for GCC/Clang
    include(ProcessorCount)
    ProcessorCount(N)
    if(NOT N EQUAL 0)
        set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
        message(STATUS "Parallel build enabled with ${N} cores")
    endif()
endif()

# **Find Qt6 with required components**
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Test Concurrent)

# **Build options**
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_TESTS "Build test applications" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_COMMAND_SYSTEM "Build Command-based UI system" ON)
option(BUILD_ADAPTERS "Build integration adapters" ON)
option(ENABLE_COMMAND_DEBUG "Enable Command system debug output" OFF)

# **Performance and Optimization Options**
option(ENABLE_LTO "Enable Link Time Optimization" OFF)
option(ENABLE_UNITY_BUILD "Enable Unity/Jumbo builds for faster compilation" OFF)
option(ENABLE_CCACHE "Enable ccache for faster rebuilds" ON)
option(ENABLE_PRECOMPILED_HEADERS "Enable precompiled headers" ON)

# **Enable CTest at top-level so tests in subdirectories are discoverable**
enable_testing()

# **Performance Optimization Setup**
# Enable ccache if available and requested
if(ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        message(STATUS "ccache found and enabled: ${CCACHE_PROGRAM}")
    else()
        message(STATUS "ccache requested but not found")
    endif()
endif()

# **Link Time Optimization**
if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "Link Time Optimization (LTO) enabled")
    else()
        message(WARNING "LTO requested but not supported: ${ipo_error}")
    endif()
endif()

# **Compiler-specific optimizations**
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

    # MSVC-specific optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /Ob2 /DNDEBUG)
        # Enable function-level linking for better optimization
        add_compile_options(/Gy)
        add_link_options(/OPT:REF /OPT:ICF)
    endif()

    # Architecture-specific optimizations
    if(DECLARATIVE_UI_ARCH STREQUAL "x64")
        add_compile_options(/favor:AMD64)
    elseif(DECLARATIVE_UI_ARCH STREQUAL "arm64")
        add_compile_options(/favor:ARM64)
    endif()
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

    # GCC/Clang-specific optimizations
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
        # Enable native architecture optimizations if not cross-compiling
        if(NOT CMAKE_CROSSCOMPILING)
            add_compile_options(-march=native -mtune=native)
        endif()
    endif()

    # Architecture-specific optimizations
    if(DECLARATIVE_UI_ARCH STREQUAL "arm64")
        add_compile_options(-mcpu=native)
    endif()
endif()

# **Debug/Release configurations**
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DECLARATIVE_UI_DEBUG)
    # Enable debug optimizations
    if(MSVC)
        add_compile_options(/Od /RTC1)
    else()
        add_compile_options(-Og -g3)
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_compile_definitions(DECLARATIVE_UI_DEBUG)
    if(MSVC)
        add_compile_options(/O2 /Zi)
    else()
        add_compile_options(-O2 -g)
    endif()
endif()

# **Create main library first**
add_library(DeclarativeUI STATIC
    # Core (excluding UIElement.cpp which is now in Core library)
    src/Core/CacheManager.cpp
    src/Core/MemoryManager.cpp
    src/Core/ParallelProcessor.cpp

    # Debug Components
    src/Debug/ProfilerDashboard.cpp
    src/Debug/DebuggingConsole.cpp
    src/Debug/BottleneckDetectorWidget.cpp
    src/Debug/PerformanceProfilerWidget.cpp

    # JSON Support
    src/JSON/JSONUILoader.cpp
    src/JSON/JSONParser.cpp
    src/JSON/JSONValidator.cpp
    src/JSON/ComponentRegistry.cpp

    # Hot Reload
    src/HotReload/FileWatcher.cpp
    src/HotReload/HotReloadManager.cpp
    src/HotReload/PerformanceMonitor.cpp

    # Binding
    src/Binding/StateManager.cpp
    src/Binding/PropertyBinding.cpp

    # Command System - Core
    src/Command/CommandSystem.cpp
    src/Command/BuiltinCommands.cpp
    src/Command/CommandIntegration.cpp
    src/Command/UICommand.cpp
    src/Command/UICommandFactory.cpp
    src/Command/WidgetMapper.cpp

    # Command System - Core Commands
    src/Command/CoreCommands.cpp
    src/Command/SpecializedCommands.cpp
    src/Command/ComponentCommands.cpp
    src/Command/ComponentCommandsExtended.cpp
    src/Command/ComponentCommandsInput.cpp
    src/Command/ComponentCommandsDisplay.cpp
    src/Command/ComponentCommandsView.cpp
    src/Command/ComponentCommandsContainer.cpp
    src/Command/ComponentCommandsContainerExtended.cpp
    src/Command/ComponentCommandsMenu.cpp
    src/Command/ComponentCommandsMenuExtended.cpp

    # Command System - Builders and Binding
    src/Command/CommandBuilder.cpp
    src/Command/CommandBinding.cpp

    # Command System - Events
    src/Command/CommandEvents.cpp

    # Command System - MVC Integration
    src/Command/MVCIntegration.cpp

    # Command System - Adapters (conditional)
    $<$<BOOL:${BUILD_ADAPTERS}>:src/Command/Adapters/UIElementAdapter.cpp>
    $<$<BOOL:${BUILD_ADAPTERS}>:src/Command/Adapters/JSONCommandLoader.cpp>
    $<$<BOOL:${BUILD_ADAPTERS}>:src/Command/Adapters/StateManagerAdapter.cpp>
    $<$<BOOL:${BUILD_ADAPTERS}>:src/Command/Adapters/ComponentSystemAdapter.cpp>
    $<$<BOOL:${BUILD_ADAPTERS}>:src/Command/Adapters/IntegrationManager.cpp>
)

# **Add subdirectories before main library linking**
add_subdirectory(src/Animation)
add_subdirectory(src/Binding)
add_subdirectory(src/Core)
add_subdirectory(src/Components)

target_link_libraries(DeclarativeUI
    Animation
    Core
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

target_include_directories(DeclarativeUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# **Enable Qt MOC for main library**
set_target_properties(DeclarativeUI PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)

# **Unity Build Configuration**
if(ENABLE_UNITY_BUILD)
    set_target_properties(DeclarativeUI PROPERTIES
        UNITY_BUILD ON
        UNITY_BUILD_BATCH_SIZE 16
    )
    message(STATUS "Unity build enabled for DeclarativeUI")
endif()

# **Precompiled Headers**
if(ENABLE_PRECOMPILED_HEADERS)
    target_precompile_headers(DeclarativeUI PRIVATE
        <QtCore/QtCore>
        <QtWidgets/QtWidgets>
        <QtNetwork/QtNetwork>
        <memory>
        <vector>
        <string>
        <unordered_map>
        <functional>
    )
    message(STATUS "Precompiled headers enabled for DeclarativeUI")
endif()

# **Apply platform-specific optimizations and advanced build options**
configure_platform_optimizations(DeclarativeUI)
configure_qt6_platform(DeclarativeUI)
configure_performance_monitoring(DeclarativeUI)
configure_advanced_build_options(DeclarativeUI)

# **Command System Configuration**
if(BUILD_COMMAND_SYSTEM)
    target_compile_definitions(DeclarativeUI PUBLIC DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED)
    message(STATUS "Command-based UI system enabled")
endif()

if(BUILD_ADAPTERS)
    target_compile_definitions(DeclarativeUI PUBLIC DECLARATIVE_UI_ADAPTERS_ENABLED)
    message(STATUS "Integration adapters enabled")
endif()

if(ENABLE_COMMAND_DEBUG)
    target_compile_definitions(DeclarativeUI PUBLIC DECLARATIVE_UI_COMMAND_DEBUG)
    message(STATUS "Command system debug output enabled")
endif()

# **Copy resources to build directory**
add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_BINARY_DIR}/Resources
)

# **Conditionally build examples**
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# **Conditionally build tests**
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
