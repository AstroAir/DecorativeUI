# **DeclarativeUI Examples Build Configuration**
# This file contains all example applications organized by learning progression
# Examples are built conditionally based on BUILD_EXAMPLES option

# **Enable Qt MOC processing for all examples**
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# **Add subdirectories for example categories**
# Organized by learning progression from beginner to advanced
add_subdirectory(01-getting-started)
add_subdirectory(02-fundamentals)
add_subdirectory(03-components)
add_subdirectory(04-advanced-features)
add_subdirectory(05-command-system)
add_subdirectory(06-applications)
add_subdirectory(enhanced-components)

# **Copy shared resources for examples**
add_custom_target(CopyExampleResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_BINARY_DIR}/examples/Resources
)

# **Copy example-specific shared resources**
add_custom_target(CopyExampleSharedResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shared
    ${CMAKE_BINARY_DIR}/examples/shared
)

message(STATUS "DeclarativeUI Examples configured with new organized structure")
