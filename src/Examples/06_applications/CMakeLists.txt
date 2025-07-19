# **Real-World Applications (36-40)**
# Complete application examples demonstrating real-world usage

# **36 - Todo App**
add_executable(36_todo_app 36_todo_app.cpp)
target_link_libraries(36_todo_app DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **37 - Form Example**
add_executable(37_form_example 37_form_example.cpp)
target_link_libraries(37_form_example DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **38 - Settings Example**
add_executable(38_settings_example 38_settings_example.cpp)
target_link_libraries(38_settings_example DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **39 - Dashboard Example**
add_executable(39_dashboard_example 39_dashboard_example.cpp)
target_link_libraries(39_dashboard_example DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **TODO: Additional application examples to be implemented**
# 40 - Text Editor (Simple text editor with hot reload)
# 41 - Plugin System (Extensible plugin architecture)

# **Set output directory**
set_target_properties(
    36_todo_app
    37_form_example
    38_settings_example
    39_dashboard_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/06_applications
)

# **Copy resources (only if directory exists)**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_target(CopyApplicationExampleResources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/examples/06_applications/resources
    )

    # **Dependencies**
    add_dependencies(36_todo_app CopyApplicationExampleResources)
    add_dependencies(37_form_example CopyApplicationExampleResources)
    add_dependencies(38_settings_example CopyApplicationExampleResources)
    add_dependencies(39_dashboard_example CopyApplicationExampleResources)
endif()
