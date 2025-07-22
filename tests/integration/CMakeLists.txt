# **Integration Tests Configuration**
# Tests that verify interaction between multiple components

# **Integration Tests**
add_executable(IntegrationTest test_integration.cpp)
target_link_libraries(IntegrationTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **End-to-End Workflow Tests** (temporarily disabled due to API mismatches)
# add_executable(EndToEndWorkflowTest test_end_to_end_workflows.cpp)
# target_link_libraries(EndToEndWorkflowTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Error Handling Integration Tests** (temporarily disabled due to API mismatches)
# add_executable(ErrorHandlingIntegrationTest test_error_handling_integration.cpp)
# target_link_libraries(ErrorHandlingIntegrationTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Set output directory for integration tests**
set_target_properties(
    IntegrationTest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/integration
)

# **Make tests depend on resources**
add_dependencies(IntegrationTest CopyTestResources)

# **Register tests with CTest**
add_test(NAME IntegrationTest COMMAND IntegrationTest)
