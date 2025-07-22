# **Tests Build Configuration**
# This file contains all test applications
# Tests are built conditionally based on BUILD_TESTS option

# **Enable Qt MOC processing for all tests**
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# **Add subdirectories for test categories**
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(command)
add_subdirectory(performance)

# **Copy resources for tests**
add_custom_target(CopyTestResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_BINARY_DIR}/tests/Resources
)

# **Enable CTest integration**
enable_testing()
