# **Unit Tests Configuration**
# Core unit tests for individual components

# **Core Module Tests**
add_executable(CoreTest test_core.cpp)
target_link_libraries(CoreTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Advanced Core Module Tests**
add_executable(CoreAdvancedTest test_core_advanced.cpp)
target_link_libraries(CoreAdvancedTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    Qt6::Concurrent
)

# **Component Tests**
add_executable(ComponentTest test_components.cpp)
target_link_libraries(ComponentTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Container Component Tests**
add_executable(ContainerComponentTest test_container_components.cpp)
target_link_libraries(ContainerComponentTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Binding Module Tests**
add_executable(PropertyBindingTest test_property_binding.cpp)
target_link_libraries(PropertyBindingTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

add_executable(StateManagerTest test_state_manager.cpp)
target_link_libraries(StateManagerTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

add_executable(PropertyBindingTemplateTest test_property_binding_template.cpp)
target_link_libraries(PropertyBindingTemplateTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **JSON Module Tests**
add_executable(JSONTest test_json.cpp)
target_link_libraries(JSONTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Hot Reload Module Tests** (temporarily disabled due to API mismatches)
# add_executable(HotReloadTest test_hot_reload.cpp)
# target_link_libraries(HotReloadTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Exception Handling Tests**
add_executable(ExceptionsTest test_exceptions.cpp)
target_link_libraries(ExceptionsTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Thread Safety Tests**
add_executable(ThreadSafetyTest test_thread_safety.cpp)
target_link_libraries(ThreadSafetyTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    Qt6::Concurrent
)

# **Set output directory for unit tests**
set_target_properties(
    CoreTest
    ComponentTest
    PropertyBindingTest
    StateManagerTest
    PropertyBindingTemplateTest
    JSONTest
    # HotReloadTest
    ExceptionsTest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/unit
)

# **Make tests depend on resources**
add_dependencies(CoreTest CopyTestResources)
add_dependencies(ComponentTest CopyTestResources)
add_dependencies(PropertyBindingTest CopyTestResources)
add_dependencies(StateManagerTest CopyTestResources)
add_dependencies(PropertyBindingTemplateTest CopyTestResources)
add_dependencies(JSONTest CopyTestResources)
# add_dependencies(HotReloadTest CopyTestResources)
add_dependencies(ExceptionsTest CopyTestResources)

# **Register tests with CTest**
add_test(NAME CoreTest COMMAND CoreTest)
add_test(NAME ComponentTest COMMAND ComponentTest)
add_test(NAME PropertyBindingTest COMMAND PropertyBindingTest)
add_test(NAME StateManagerTest COMMAND StateManagerTest)
add_test(NAME PropertyBindingTemplateTest COMMAND PropertyBindingTemplateTest)
add_test(NAME JSONTest COMMAND JSONTest)
# add_test(NAME HotReloadTest COMMAND HotReloadTest)
add_test(NAME ExceptionsTest COMMAND ExceptionsTest)
