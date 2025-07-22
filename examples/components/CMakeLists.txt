# **Component Examples Configuration**
# Examples demonstrating UI component usage

# **Set output directory for component examples**
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/components)

# **Basic Components Example**
add_executable(BasicComponentsExample 06_basic_components.cpp)
target_include_directories(BasicComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(BasicComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Input Components Example**
add_executable(InputComponentsExample 07_input_components.cpp)
target_include_directories(InputComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(InputComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Comprehensive Components Example**
add_executable(ComprehensiveComponentsExample 08_comprehensive_components.cpp)
target_include_directories(ComprehensiveComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(ComprehensiveComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Enhanced Components Example**
add_executable(EnhancedComponentsExample 09_enhanced_components.cpp)
target_include_directories(EnhancedComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(EnhancedComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **New Components Example**
add_executable(NewComponentsExample 10_new_components.cpp)
target_include_directories(NewComponentsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(NewComponentsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Make examples depend on resources**
add_dependencies(BasicComponentsExample CopyExampleResources)
add_dependencies(InputComponentsExample CopyExampleResources)
add_dependencies(ComprehensiveComponentsExample CopyExampleResources)
add_dependencies(EnhancedComponentsExample CopyExampleResources)
add_dependencies(NewComponentsExample CopyExampleResources)
