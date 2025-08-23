# CMake configuration for Debug component tests
# This file can be included in the main CMakeLists.txt to add Debug component tests

# Debug component test executables
set(DEBUG_TEST_SOURCES
    test_debugging_console.cpp
    test_bottleneck_detector_widget.cpp
    test_performance_profiler_widget.cpp
)

# Create test executables for each Debug component
foreach(TEST_SOURCE ${DEBUG_TEST_SOURCES})
    # Extract test name from filename
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    # Create executable
    add_executable(${TEST_NAME} ${TEST_SOURCE})

    # Link required libraries
    target_link_libraries(${TEST_NAME}
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
        DeclarativeUI_Debug  # Assuming this is the Debug library target
    )

    # Set include directories
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/Debug
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Add to test suite
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    # Set test properties
    set_tests_properties(${TEST_NAME} PROPERTIES
        TIMEOUT 30
        LABELS "unit;debug"
    )
endforeach()

# Group tests for better organization
set_property(TEST test_debugging_console PROPERTY LABELS "unit;debug;console")
set_property(TEST test_bottleneck_detector_widget PROPERTY LABELS "unit;debug;bottleneck")
set_property(TEST test_performance_profiler_widget PROPERTY LABELS "unit;debug;profiler")

# Add custom target to run all debug tests
add_custom_target(test_debug_components
    COMMAND ${CMAKE_CTEST_COMMAND} -L "debug" --output-on-failure
    DEPENDS
        test_debugging_console
        test_bottleneck_detector_widget
        test_performance_profiler_widget
    COMMENT "Running all Debug component tests"
)
