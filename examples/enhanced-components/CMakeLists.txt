# Enhanced Components Demo
cmake_minimum_required(VERSION 3.16)

# Create the enhanced components demo executable
add_executable(EnhancedComponentsDemo
    enhanced-components-demo.cpp
)

# Link required libraries
target_link_libraries(EnhancedComponentsDemo
    DeclarativeUI
    Components
    Core
    Animation
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

# Set target properties
set_target_properties(EnhancedComponentsDemo PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(EnhancedComponentsDemo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Components
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Animation
)

# Set output directory
set_target_properties(EnhancedComponentsDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/enhanced-components
)

# Copy to examples directory for easy access
if(WIN32)
    add_custom_command(TARGET EnhancedComponentsDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:EnhancedComponentsDemo>
        ${CMAKE_BINARY_DIR}/examples/
    )
endif()
