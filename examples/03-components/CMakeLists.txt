# **Component Examples**
# Showcasing UI components and patterns

# **Basic Components Example**
# Button, Label, LineEdit demonstrations
add_executable(BasicComponentsExample basic-components/basic-components.cpp)
target_include_directories(BasicComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(BasicComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(BasicComponentsExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/03-components
)

# **Input Components Example**
# CheckBox, ComboBox, SpinBox, Slider demonstrations
add_executable(InputComponentsExample input-components/input-components.cpp)
target_include_directories(InputComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(InputComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(InputComponentsExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/03-components
)

# **Layout Components Example**
# Layout management and container components
add_executable(LayoutComponentsExample layout-components/layout-components.cpp)
target_include_directories(LayoutComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(LayoutComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(LayoutComponentsExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/03-components
)

# **Enhanced Components Example**
# Enhanced component features and usage
add_executable(EnhancedComponentsExample custom-components/enhanced-components.cpp)
target_include_directories(EnhancedComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(EnhancedComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(EnhancedComponentsExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/03-components
)

# **New Components Example**
# Latest component additions
add_executable(NewComponentsExample custom-components/new-components.cpp)
target_include_directories(NewComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(NewComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(NewComponentsExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/03-components
)

message(STATUS "Component examples configured")

