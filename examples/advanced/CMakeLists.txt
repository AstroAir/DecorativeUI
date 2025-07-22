# **Advanced Examples Configuration**
# Complex examples demonstrating advanced features

# **Set output directory for advanced examples**
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/advanced)

# **Command System Example**
add_executable(CommandSystemExample 26_command_system.cpp)
target_include_directories(CommandSystemExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandSystemExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Command Example**
add_executable(CommandExample 27_command_example.cpp)
target_include_directories(CommandExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Advanced Example**
add_executable(AdvancedExample 28_advanced_example.cpp)
target_include_directories(AdvancedExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(AdvancedExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Comprehensive Example**
add_executable(ComprehensiveExample 29_comprehensive_example.cpp)
target_include_directories(ComprehensiveExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(ComprehensiveExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Command UI Builder Example**
add_executable(CommandUIBuilderExample 30_command_ui_builder.cpp)
target_include_directories(CommandUIBuilderExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandUIBuilderExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Integration Example**
add_executable(IntegrationExampleAdvanced 31_integration_example.cpp)
target_include_directories(IntegrationExampleAdvanced PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(IntegrationExampleAdvanced
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Todo App Example**
add_executable(TodoAppExample 36_todo_app.cpp)
target_include_directories(TodoAppExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(TodoAppExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Form Example**
add_executable(FormExample 37_form_example.cpp)
target_include_directories(FormExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(FormExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Settings Example**
add_executable(SettingsExample 38_settings_example.cpp)
target_include_directories(SettingsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(SettingsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Dashboard Example**
add_executable(DashboardExample 39_dashboard_example.cpp)
target_include_directories(DashboardExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(DashboardExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Make examples depend on resources**
add_dependencies(CommandSystemExample CopyExampleResources)
add_dependencies(CommandExample CopyExampleResources)
add_dependencies(AdvancedExample CopyExampleResources)
add_dependencies(ComprehensiveExample CopyExampleResources)
add_dependencies(CommandUIBuilderExample CopyExampleResources)
add_dependencies(IntegrationExampleAdvanced CopyExampleResources)
add_dependencies(TodoAppExample CopyExampleResources)
add_dependencies(FormExample CopyExampleResources)
add_dependencies(SettingsExample CopyExampleResources)
add_dependencies(DashboardExample CopyExampleResources)
