cmake_minimum_required(VERSION 3.20)
project(DeclarativeUI VERSION 1.0.0 LANGUAGES CXX)

# **Modern C++ standard**
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# **Find Qt6 with required components**
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Test)

# **Build options**
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_TESTS "Build test applications" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# **Compiler-specific optimizations**
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# **Debug/Release configurations**
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DECLARATIVE_UI_DEBUG)
endif()

# **Create main library first**
add_library(DeclarativeUI STATIC
    # Core
    src/Core/UIElement.cpp
    src/Core/DeclarativeBuilder.cpp
    src/Core/CacheManager.cpp
    src/Core/MemoryManager.cpp
    src/Core/ParallelProcessor.cpp
    src/Animation/AnimationEngine.cpp
    src/Debug/ProfilerDashboard.cpp

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

    # Command System
    src/Command/CommandSystem.cpp
    src/Command/BuiltinCommands.cpp
    src/Command/CommandIntegration.cpp
)

target_link_libraries(DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

# **Add subdirectories after main library**
add_subdirectory(src/Components)

target_include_directories(DeclarativeUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# **Enable Qt MOC for main library**
set_target_properties(DeclarativeUI PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)

# **Copy resources to build directory**
add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_BINARY_DIR}/Resources
)

# **Conditionally build examples**
if(BUILD_EXAMPLES)
    add_subdirectory(src/Examples)
endif()

# **Conditionally build tests**
if(BUILD_TESTS)
    add_subdirectory(src/Tests)
endif()

