# **Application Examples**
# Complete sample applications combining multiple features

# **Counter App Example**
# Simple counter application
add_executable(CounterAppExample counter-app/counter-app.cpp)
target_include_directories(CounterAppExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CounterAppExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(CounterAppExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/06-applications
)

# **Todo App Example**
# Complete todo application
add_executable(TodoAppExample todo-app/todo-app.cpp)
target_include_directories(TodoAppExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(TodoAppExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(TodoAppExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/06-applications
)

# **Form Example**
# Form handling and validation
add_executable(FormExample form-example/form-example.cpp)
target_include_directories(FormExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(FormExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(FormExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/06-applications
)

# **Settings App Example**
# Settings management application
add_executable(SettingsAppExample settings-app/settings-app.cpp)
target_include_directories(SettingsAppExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(SettingsAppExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(SettingsAppExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/06-applications
)

# **Dashboard Example**
# Dashboard application
add_executable(DashboardExample dashboard/dashboard.cpp)
target_include_directories(DashboardExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(DashboardExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(DashboardExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/06-applications
)

message(STATUS "Application examples configured")
