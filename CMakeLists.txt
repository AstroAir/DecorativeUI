cmake_minimum_required(VERSION 3.20)
project(DeclarativeUI VERSION 1.0.0 LANGUAGES CXX)

# **Modern C++ standard**
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# **Find Qt6 with required components**
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Test Concurrent)

# **Build options**
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_TESTS "Build test applications" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_COMMAND_SYSTEM "Build Command-based UI system" ON)
option(BUILD_ADAPTERS "Build integration adapters" ON)
option(ENABLE_COMMAND_DEBUG "Enable Command system debug output" OFF)

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

