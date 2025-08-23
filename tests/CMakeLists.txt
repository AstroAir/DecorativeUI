# **Tests Build Configuration**
# This file contains all test applications
# Tests are built conditionally based on BUILD_TESTS option

# **Enable CTest integration early so add_test() works in subdirs**
enable_testing()

# **Enable Qt MOC processing for all tests**
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# **Add subdirectories for test categories**
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(command)
add_subdirectory(performance)

# **Standalone comprehensive component tests**
add_executable(ComponentTests ComponentTests.cpp)
target_link_libraries(ComponentTests
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Set output directory for standalone tests**
set_target_properties(ComponentTests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# **Make standalone tests depend on resources**
add_dependencies(ComponentTests CopyTestResources)

# **Copy resources for tests**
add_custom_target(CopyTestResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_BINARY_DIR}/tests/Resources
)

# **Enable CTest integration**
enable_testing()

# **Register standalone tests with CTest**
add_test(NAME ComponentTests COMMAND ComponentTests)
