# **Hot Reload Examples (21-25)**
# Advanced hot reload features and development workflow optimization

# **21 - File Watching**
add_executable(21_file_watching 21_file_watching.cpp)
target_link_libraries(21_file_watching DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **22 - Hot Reload Example**
add_executable(22_hot_reload_example 22_hot_reload_example.cpp)
target_link_libraries(22_hot_reload_example DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **TODO: Additional hot reload examples to be implemented**
# 23 - Reload Strategies (Different reload strategies and error handling)
# 24 - Performance Monitoring (Hot reload performance optimization)
# 25 - Production Hot Reload (Hot reload in production environments)

# **Set output directory**
set_target_properties(
    21_file_watching
    22_hot_reload_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04_hot_reload
)

# **Copy resources (only if directory exists)**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_target(CopyHotReloadExampleResources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/examples/04_hot_reload/resources
    )

    # **Dependencies**
    add_dependencies(21_file_watching CopyHotReloadExampleResources)
    add_dependencies(22_hot_reload_example CopyHotReloadExampleResources)
endif()
