# **Advanced Features Examples (26-35)**
# Complex application patterns and advanced functionality

# **26 - Command System**
add_executable(26_command_system 26_command_system.cpp)
target_link_libraries(26_command_system DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **27 - Command Example**
add_executable(27_command_example 27_command_example.cpp)
target_link_libraries(27_command_example DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **28 - Advanced Example**
add_executable(28_advanced_example 28_advanced_example.cpp)
target_link_libraries(28_advanced_example DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **29 - Comprehensive Example**
add_executable(29_comprehensive_example 29_comprehensive_example.cpp)
target_link_libraries(29_comprehensive_example DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **TODO: Additional advanced examples to be implemented**
# 30 - Form Validation (Complex form validation)
# 31 - Async Operations (Asynchronous operations and loading states)
# 32 - Error Handling (Comprehensive error handling strategies)
# 33 - Testing Integration (Testing DeclarativeUI applications)
# 34 - Performance Optimization (Performance best practices)
# 35 - Accessibility (Accessibility features and compliance)

# **Set output directory**
set_target_properties(
    26_command_system
    27_command_example
    28_advanced_example
    29_comprehensive_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05_advanced
)

# **Copy resources (only if directory exists)**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_target(CopyAdvancedExampleResources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/examples/05_advanced/resources
    )

    # **Dependencies**
    add_dependencies(26_command_system CopyAdvancedExampleResources)
    add_dependencies(27_command_example CopyAdvancedExampleResources)
    add_dependencies(28_advanced_example CopyAdvancedExampleResources)
    add_dependencies(29_comprehensive_example CopyAdvancedExampleResources)
endif()
