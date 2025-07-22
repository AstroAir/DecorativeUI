# **Performance Tests Configuration**
# Benchmarking and performance regression tests

# **Component Performance Tests**
add_executable(ComponentPerformanceTest test_component_performance.cpp)
target_link_libraries(ComponentPerformanceTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    Qt6::Concurrent
)

# **Set output directory for performance tests**
set_target_properties(
    ComponentPerformanceTest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/performance
)

# **Make tests depend on resources**
add_dependencies(ComponentPerformanceTest CopyTestResources)

# **Register performance tests with CTest**
add_test(NAME ComponentPerformanceTest COMMAND ComponentPerformanceTest)

# **Set test properties for performance tests**
set_tests_properties(ComponentPerformanceTest PROPERTIES
    TIMEOUT 300  # 5 minutes timeout for performance tests
    LABELS "performance;benchmark"
)
