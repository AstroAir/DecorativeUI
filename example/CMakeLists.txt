cmake_minimum_required(VERSION 3.20)
project(DeclarativeUIShowcase VERSION 1.0.0 LANGUAGES CXX)

# Modern C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Qt6 with required components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Charts)

# Find DeclarativeUI (assume it's built in parent directory)
set(DECLARATIVEUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(DECLARATIVEUI_INCLUDE_DIR ${DECLARATIVEUI_ROOT}/src)
set(DECLARATIVEUI_BUILD_DIR ${DECLARATIVEUI_ROOT}/build)

# Include DeclarativeUI headers
include_directories(${DECLARATIVEUI_INCLUDE_DIR})

# Link directories
link_directories(${DECLARATIVEUI_BUILD_DIR})

# Compiler-specific optimizations
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DECLARATIVE_UI_DEBUG)
endif()

# Enhanced Components Library
add_library(EnhancedComponents STATIC
    components/SearchBox.cpp
    components/DataTable.cpp
    components/Chart.cpp
    components/NavigationBar.cpp
    components/StatusIndicator.cpp
    components/EnhancedComponents.cpp
)

target_include_directories(EnhancedComponents PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DECLARATIVEUI_INCLUDE_DIR}
)

target_link_libraries(EnhancedComponents
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
)

set_target_properties(EnhancedComponents PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)

# Utilities Library
add_library(ShowcaseUtils STATIC
    utils/SampleDataGenerator.cpp
    utils/PerformanceProfiler.cpp
    utils/ExampleHelpers.cpp
)

target_include_directories(ShowcaseUtils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DECLARATIVEUI_INCLUDE_DIR}
)

target_link_libraries(ShowcaseUtils
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(ShowcaseUtils PROPERTIES
    AUTOMOC ON
)

# Main Showcase Application
add_executable(DeclarativeUIShowcase
    main.cpp
    showcase/ShowcaseApp.cpp
    showcase/ComponentGallery.cpp
    showcase/StateDemo.cpp
    showcase/AnimationDemo.cpp
    showcase/CommandDemo.cpp
    showcase/ThemeManager.cpp
    showcase/PerformanceMonitor.cpp
)

target_include_directories(DeclarativeUIShowcase PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DECLARATIVEUI_INCLUDE_DIR}
)

target_link_libraries(DeclarativeUIShowcase
    EnhancedComponents
    ShowcaseUtils
    DeclarativeUI
    Animation
    Core
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
)

set_target_properties(DeclarativeUIShowcase PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Copy resources to build directory
add_custom_target(CopyShowcaseResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
    COMMENT "Copying showcase resources"
)

add_dependencies(DeclarativeUIShowcase CopyShowcaseResources)

# Installation
install(TARGETS DeclarativeUIShowcase
    RUNTIME DESTINATION bin
)

install(DIRECTORY resources/
    DESTINATION share/DeclarativeUIShowcase/resources
)

message(STATUS "DeclarativeUI Showcase configured successfully")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "DeclarativeUI root: ${DECLARATIVEUI_ROOT}")
