# **Basic Examples Configuration**
# Simple examples demonstrating core functionality

# **Set output directory for basic examples**
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/basic)

# **Hello World Example**
add_executable(HelloWorldExample 01_hello_world.cpp)
target_include_directories(HelloWorldExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(HelloWorldExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **JSON UI Loading Example**
add_executable(JSONUILoadingExample 02_json_ui_loading.cpp)
target_include_directories(JSONUILoadingExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(JSONUILoadingExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Simple State Example**
add_executable(SimpleStateExample 03_simple_state.cpp)
target_include_directories(SimpleStateExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(SimpleStateExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Basic Hot Reload Example**
add_executable(BasicHotReloadExample 04_basic_hot_reload.cpp)
target_include_directories(BasicHotReloadExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(BasicHotReloadExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Event Handling Example**
add_executable(EventHandlingExample 05_event_handling.cpp)
target_include_directories(EventHandlingExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(EventHandlingExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Counter App Example**
add_executable(CounterAppExample 06_counter_app.cpp)
target_include_directories(CounterAppExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CounterAppExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Command Basics Example**
add_executable(CommandBasicsExample 08_command_basics.cpp)
target_include_directories(CommandBasicsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandBasicsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Command Basics Simple Example**
add_executable(CommandBasicsSimpleExample 08_command_basics_simple.cpp)
target_include_directories(CommandBasicsSimpleExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandBasicsSimpleExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Reactive State Example**
add_executable(ReactiveStateExample 16_reactive_state.cpp)
target_include_directories(ReactiveStateExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(ReactiveStateExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **File Watching Example**
add_executable(FileWatchingExample 21_file_watching.cpp)
target_include_directories(FileWatchingExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(FileWatchingExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Hot Reload Example**
add_executable(HotReloadExample 22_hot_reload_example.cpp)
target_include_directories(HotReloadExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(HotReloadExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

# **Make examples depend on resources**
add_dependencies(HelloWorldExample CopyExampleResources)
add_dependencies(JSONUILoadingExample CopyExampleResources)
add_dependencies(SimpleStateExample CopyExampleResources)
add_dependencies(BasicHotReloadExample CopyExampleResources)
add_dependencies(EventHandlingExample CopyExampleResources)
add_dependencies(CounterAppExample CopyExampleResources)
add_dependencies(CommandBasicsExample CopyExampleResources)
add_dependencies(CommandBasicsSimpleExample CopyExampleResources)
add_dependencies(ReactiveStateExample CopyExampleResources)
add_dependencies(FileWatchingExample CopyExampleResources)
add_dependencies(HotReloadExample CopyExampleResources)
