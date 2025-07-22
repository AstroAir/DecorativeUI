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

# **30 - Command UI Builder (Conditional)**
if(BUILD_COMMAND_SYSTEM)
    add_executable(30_command_ui_builder 30_command_ui_builder.cpp)
    target_link_libraries(30_command_ui_builder DeclarativeUI Components Qt6::Core Qt6::Widgets)
    target_compile_definitions(30_command_ui_builder PRIVATE DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED)

    if(BUILD_ADAPTERS)
        target_compile_definitions(30_command_ui_builder PRIVATE DECLARATIVE_UI_ADAPTERS_ENABLED)
    endif()

    message(STATUS "Command UI Builder example will be built")
endif()

# **31 - Integration Example (Conditional)**
if(BUILD_COMMAND_SYSTEM AND BUILD_ADAPTERS)
    add_executable(31_integration_example 31_integration_example.cpp)
    target_link_libraries(31_integration_example DeclarativeUI Components Qt6::Core Qt6::Widgets)
    target_compile_definitions(31_integration_example PRIVATE
        DECLARATIVE_UI_COMMAND_SYSTEM_ENABLED
        DECLARATIVE_UI_ADAPTERS_ENABLED
    )

    message(STATUS "Integration example will be built")
endif()

# **TODO: Additional advanced examples to be implemented**
# 32 - Form Validation (Complex form validation)
# 33 - Async Operations (Asynchronous operations and loading states)
# 34 - Error Handling (Comprehensive error handling strategies)
# 35 - Testing Integration (Testing DeclarativeUI applications)
# 36 - Performance Optimization (Performance best practices)
# 37 - Accessibility (Accessibility features and compliance)

# **Set output directory**
set_target_properties(
    26_command_system
    27_command_example
    28_advanced_example
    29_comprehensive_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05_advanced
)

# **Set output directory for Command examples (conditional)**
if(BUILD_COMMAND_SYSTEM)
    set_target_properties(30_command_ui_builder
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05_advanced
    )
endif()

if(BUILD_COMMAND_SYSTEM AND BUILD_ADAPTERS)
    set_target_properties(31_integration_example
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05_advanced
    )
endif()

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

    # **Command example dependencies (conditional)**
    if(BUILD_COMMAND_SYSTEM)
        add_dependencies(30_command_ui_builder CopyAdvancedExampleResources)
    endif()

    if(BUILD_COMMAND_SYSTEM AND BUILD_ADAPTERS)
        add_dependencies(31_integration_example CopyAdvancedExampleResources)
    endif()
endif()
