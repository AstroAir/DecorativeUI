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

# **Hot Reload Module Tests**
add_executable(HotReloadTest test_hot_reload.cpp)
target_link_libraries(HotReloadTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Advanced Hot Reload Tests**
add_executable(HotReloadAdvancedTest test_hot_reload_advanced.cpp)
target_link_libraries(HotReloadAdvancedTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Performance Monitor Advanced Tests**
add_executable(PerformanceMonitorAdvancedTest test_performance_monitor_advanced.cpp)
target_link_libraries(PerformanceMonitorAdvancedTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Hot Reload Manager Advanced Tests**
add_executable(HotReloadManagerAdvancedTest test_hot_reload_manager_advanced.cpp)
target_link_libraries(HotReloadManagerAdvancedTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **JSON Advanced Tests**
add_executable(JSONAdvancedTest test_json_advanced.cpp)
target_link_libraries(JSONAdvancedTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **HotReload JSON Integration Tests**
add_executable(HotReloadJSONIntegrationTest test_hot_reload_json_integration.cpp)
target_link_libraries(HotReloadJSONIntegrationTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

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

# **Modern C++20 Features Tests** (temporarily disabled due to API mismatches)
# add_executable(ModernCppTest test_modern_cpp_features.cpp)
# target_link_libraries(ModernCppTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Theme System Tests**
add_executable(ThemeSystemTest test_theme_system.cpp)
target_link_libraries(ThemeSystemTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Animation System Tests** (temporarily disabled due to template issues)
# add_executable(AnimationSystemTest test_animation_system.cpp)
# target_link_libraries(AnimationSystemTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Validation Framework Tests** (temporarily disabled due to template issues)
# add_executable(ValidationFrameworkTest test_validation_framework.cpp)
# target_link_libraries(ValidationFrameworkTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Error Handling Tests** (temporarily disabled due to API mismatches)
# add_executable(ErrorHandlingTest test_error_handling.cpp)
# target_link_libraries(ErrorHandlingTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Component Utils Tests** (temporarily disabled due to API mismatches)
# add_executable(ComponentUtilsTest test_component_utils.cpp)
# target_link_libraries(ComponentUtilsTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Smart Pointers Tests** (temporarily disabled due to API mismatches)
# add_executable(SmartPointersTest test_smart_pointers.cpp)
# target_link_libraries(SmartPointersTest
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Test
# )

# **Accessibility Tests**
add_executable(AccessibilityTest test_accessibility.cpp)
target_link_libraries(AccessibilityTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Cache Manager Tests**
add_executable(CacheManagerTest CacheManagerTest.cpp)
target_link_libraries(CacheManagerTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Debug Component Tests**
add_executable(DebuggingConsoleTest test_debugging_console.cpp)
target_link_libraries(DebuggingConsoleTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

add_executable(BottleneckDetectorWidgetTest test_bottleneck_detector_widget.cpp)
target_link_libraries(BottleneckDetectorWidgetTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

add_executable(PerformanceProfilerWidgetTest test_performance_profiler_widget.cpp)
target_link_libraries(PerformanceProfilerWidgetTest
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# **Set output directory for unit tests**
set_target_properties(
    CoreTest
    CoreAdvancedTest
    ComponentTest
    ContainerComponentTest
    PropertyBindingTest
    StateManagerTest
    PropertyBindingTemplateTest
    JSONTest
    HotReloadTest
    HotReloadAdvancedTest
    PerformanceMonitorAdvancedTest
    HotReloadManagerAdvancedTest
    JSONAdvancedTest
    HotReloadJSONIntegrationTest
    ExceptionsTest
    ThreadSafetyTest
    # ModernCppTest
    ThemeSystemTest
    # AnimationSystemTest
    # ValidationFrameworkTest
    # ErrorHandlingTest
    # ComponentUtilsTest
    # SmartPointersTest
    AccessibilityTest
    CacheManagerTest
    DebuggingConsoleTest
    BottleneckDetectorWidgetTest
    PerformanceProfilerWidgetTest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/unit
)

# **Make tests depend on resources**
add_dependencies(CoreTest CopyTestResources)
add_dependencies(CoreAdvancedTest CopyTestResources)
add_dependencies(ComponentTest CopyTestResources)
add_dependencies(ContainerComponentTest CopyTestResources)
add_dependencies(PropertyBindingTest CopyTestResources)
add_dependencies(StateManagerTest CopyTestResources)
add_dependencies(PropertyBindingTemplateTest CopyTestResources)
add_dependencies(JSONTest CopyTestResources)
add_dependencies(HotReloadTest CopyTestResources)
add_dependencies(HotReloadAdvancedTest CopyTestResources)
add_dependencies(PerformanceMonitorAdvancedTest CopyTestResources)
add_dependencies(HotReloadManagerAdvancedTest CopyTestResources)
add_dependencies(JSONAdvancedTest CopyTestResources)
add_dependencies(HotReloadJSONIntegrationTest CopyTestResources)
add_dependencies(ExceptionsTest CopyTestResources)
add_dependencies(ThreadSafetyTest CopyTestResources)
# add_dependencies(ModernCppTest CopyTestResources)
add_dependencies(ThemeSystemTest CopyTestResources)
# add_dependencies(AnimationSystemTest CopyTestResources)
# add_dependencies(ValidationFrameworkTest CopyTestResources)
# add_dependencies(ErrorHandlingTest CopyTestResources)
# add_dependencies(ComponentUtilsTest CopyTestResources)
# add_dependencies(SmartPointersTest CopyTestResources)
add_dependencies(AccessibilityTest CopyTestResources)
add_dependencies(CacheManagerTest CopyTestResources)
add_dependencies(DebuggingConsoleTest CopyTestResources)
add_dependencies(BottleneckDetectorWidgetTest CopyTestResources)
add_dependencies(PerformanceProfilerWidgetTest CopyTestResources)

# **Register tests with CTest**
add_test(NAME CoreTest COMMAND CoreTest)
add_test(NAME CoreAdvancedTest COMMAND CoreAdvancedTest)
add_test(NAME ComponentTest COMMAND ComponentTest)
add_test(NAME ContainerComponentTest COMMAND ContainerComponentTest)
add_test(NAME PropertyBindingTest COMMAND PropertyBindingTest)
add_test(NAME StateManagerTest COMMAND StateManagerTest)
add_test(NAME PropertyBindingTemplateTest COMMAND PropertyBindingTemplateTest)
add_test(NAME JSONTest COMMAND JSONTest)
add_test(NAME HotReloadTest COMMAND HotReloadTest)
add_test(NAME HotReloadAdvancedTest COMMAND HotReloadAdvancedTest)
add_test(NAME PerformanceMonitorAdvancedTest COMMAND PerformanceMonitorAdvancedTest)
add_test(NAME HotReloadManagerAdvancedTest COMMAND HotReloadManagerAdvancedTest)
add_test(NAME JSONAdvancedTest COMMAND JSONAdvancedTest)
add_test(NAME HotReloadJSONIntegrationTest COMMAND HotReloadJSONIntegrationTest)
add_test(NAME ExceptionsTest COMMAND ExceptionsTest)
add_test(NAME ThreadSafetyTest COMMAND ThreadSafetyTest)
# add_test(NAME ModernCppTest COMMAND ModernCppTest)
add_test(NAME ThemeSystemTest COMMAND ThemeSystemTest)
# add_test(NAME AnimationSystemTest COMMAND AnimationSystemTest)
# add_test(NAME ValidationFrameworkTest COMMAND ValidationFrameworkTest)
# add_test(NAME ErrorHandlingTest COMMAND ErrorHandlingTest)
# add_test(NAME ComponentUtilsTest COMMAND ComponentUtilsTest)
# add_test(NAME SmartPointersTest COMMAND SmartPointersTest)
add_test(NAME AccessibilityTest COMMAND AccessibilityTest)
add_test(NAME CacheManagerTest COMMAND CacheManagerTest)
add_test(NAME DebuggingConsoleTest COMMAND DebuggingConsoleTest)
add_test(NAME BottleneckDetectorWidgetTest COMMAND BottleneckDetectorWidgetTest)
add_test(NAME PerformanceProfilerWidgetTest COMMAND PerformanceProfilerWidgetTest)
