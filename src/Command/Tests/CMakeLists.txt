# **Command System Tests**

# **Find required packages**
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

# **Enable Qt MOC processing for all tests**
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# **Command System Core Tests**
add_executable(CommandSystemCoreTest
    test_command_core.cpp
)

target_link_libraries(CommandSystemCoreTest
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(CommandSystemCoreTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# **Command Builder Tests**
add_executable(CommandBuilderTest
    test_command_builder.cpp
)

target_link_libraries(CommandBuilderTest
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(CommandBuilderTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# **Command State Tests**
add_executable(CommandStateTest
    test_command_state.cpp
)

target_link_libraries(CommandStateTest
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(CommandStateTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# **Widget Mapper Tests**
add_executable(WidgetMapperTest
    test_widget_mapper.cpp
)

target_link_libraries(WidgetMapperTest
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(WidgetMapperTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# **Command Events Tests**
add_executable(CommandEventsTest
    test_command_events.cpp
)

target_link_libraries(CommandEventsTest
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(CommandEventsTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# **Command Binding Tests**
add_executable(CommandBindingTest
    test_command_binding.cpp
)

target_link_libraries(CommandBindingTest
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(CommandBindingTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# **MVC Integration Tests**
add_executable(MVCIntegrationTest
    test_mvc_integration.cpp
)

target_link_libraries(MVCIntegrationTest
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(MVCIntegrationTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# **Adapter Tests (if adapters are enabled)**
if(BUILD_ADAPTERS)
    # **UIElement Adapter Tests**
    add_executable(UIElementAdapterTest
        test_uielement_adapter.cpp
    )
    
    target_link_libraries(UIElementAdapterTest
        DeclarativeUI
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
    )
    
    target_include_directories(UIElementAdapterTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    
    # **JSON Command Loader Tests**
    add_executable(JSONCommandLoaderTest
        test_json_command_loader.cpp
    )
    
    target_link_libraries(JSONCommandLoaderTest
        DeclarativeUI
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
    )
    
    target_include_directories(JSONCommandLoaderTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    
    # **State Manager Adapter Tests**
    add_executable(StateManagerAdapterTest
        test_state_manager_adapter.cpp
    )
    
    target_link_libraries(StateManagerAdapterTest
        DeclarativeUI
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
    )
    
    target_include_directories(StateManagerAdapterTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    
    # **Component System Adapter Tests**
    add_executable(ComponentSystemAdapterTest
        test_component_system_adapter.cpp
    )
    
    target_link_libraries(ComponentSystemAdapterTest
        DeclarativeUI
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
    )
    
    target_include_directories(ComponentSystemAdapterTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    
    # **Integration Manager Tests**
    add_executable(IntegrationManagerTest
        test_integration_manager.cpp
    )
    
    target_link_libraries(IntegrationManagerTest
        DeclarativeUI
        Qt6::Core
        Qt6::Widgets
        Qt6::Test
    )
    
    target_include_directories(IntegrationManagerTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    
    message(STATUS "Command adapter tests configured")
endif()

# **Set output directory for all tests**
set_target_properties(
    CommandSystemCoreTest
    CommandBuilderTest
    CommandStateTest
    WidgetMapperTest
    CommandEventsTest
    CommandBindingTest
    MVCIntegrationTest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

if(BUILD_ADAPTERS)
    set_target_properties(
        UIElementAdapterTest
        JSONCommandLoaderTest
        StateManagerAdapterTest
        ComponentSystemAdapterTest
        IntegrationManagerTest
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endif()

# **Enable CTest integration**
enable_testing()

# **Register core tests**
add_test(NAME CommandSystemCoreTest COMMAND CommandSystemCoreTest)
add_test(NAME CommandBuilderTest COMMAND CommandBuilderTest)
add_test(NAME CommandStateTest COMMAND CommandStateTest)
add_test(NAME WidgetMapperTest COMMAND WidgetMapperTest)
add_test(NAME CommandEventsTest COMMAND CommandEventsTest)
add_test(NAME CommandBindingTest COMMAND CommandBindingTest)
add_test(NAME MVCIntegrationTest COMMAND MVCIntegrationTest)

# **Register adapter tests**
if(BUILD_ADAPTERS)
    add_test(NAME UIElementAdapterTest COMMAND UIElementAdapterTest)
    add_test(NAME JSONCommandLoaderTest COMMAND JSONCommandLoaderTest)
    add_test(NAME StateManagerAdapterTest COMMAND StateManagerAdapterTest)
    add_test(NAME ComponentSystemAdapterTest COMMAND ComponentSystemAdapterTest)
    add_test(NAME IntegrationManagerTest COMMAND IntegrationManagerTest)
endif()

message(STATUS "Command system tests configured")
