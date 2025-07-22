# **Examples Build Configuration**
# This file contains all example applications
# Examples are built conditionally based on BUILD_EXAMPLES option

# **Enable Qt MOC processing for all examples**
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# **Include organized example subdirectories**
add_subdirectory(01_basic)
add_subdirectory(02_components)
add_subdirectory(03_state_management)
add_subdirectory(04_hot_reload)
add_subdirectory(05_advanced)
add_subdirectory(06_applications)

# **Copy resources for examples**
add_custom_target(CopyExampleResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_BINARY_DIR}/examples/Resources
)

# **Copy Command system example resources**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_target(CopyCommandExampleResources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/examples/resources
    )
    message(STATUS "Command example resources will be copied")
endif()
