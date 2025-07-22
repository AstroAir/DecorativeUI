# **Examples Build Configuration**
# This file contains all example applications
# Examples are built conditionally based on BUILD_EXAMPLES option

# **Enable Qt MOC processing for all examples**
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# **Add subdirectories for example categories**
add_subdirectory(basic)
add_subdirectory(components)
add_subdirectory(command)
add_subdirectory(advanced)

# **Copy resources for examples**
add_custom_target(CopyExampleResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_BINARY_DIR}/examples/Resources
)

# **Copy example-specific resources if they exist**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_target(CopyExampleSpecificResources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/examples/resources
    )
    message(STATUS "Example-specific resources will be copied")
endif()
