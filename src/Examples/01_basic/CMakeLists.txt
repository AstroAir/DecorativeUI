# **Basic Examples (01-05)**
# Fundamental concepts and getting started examples

# **01 - Hello World**
add_executable(01_hello_world 01_hello_world.cpp)
target_link_libraries(01_hello_world DeclarativeUI Qt6::Core Qt6::Widgets)

# **02 - JSON UI Loading**
add_executable(02_json_ui_loading 02_json_ui_loading.cpp)
target_link_libraries(02_json_ui_loading DeclarativeUI Qt6::Core Qt6::Widgets)

# **03 - Simple State**
add_executable(03_simple_state 03_simple_state.cpp)
target_link_libraries(03_simple_state DeclarativeUI Qt6::Core Qt6::Widgets)

# **04 - Basic Hot Reload**
add_executable(04_basic_hot_reload 04_basic_hot_reload.cpp)
target_link_libraries(04_basic_hot_reload DeclarativeUI Qt6::Core Qt6::Widgets)

# **05 - Event Handling**
add_executable(05_event_handling 05_event_handling.cpp)
target_link_libraries(05_event_handling DeclarativeUI Qt6::Core Qt6::Widgets)

# **06 - Counter App**
add_executable(06_counter_app 06_counter_app.cpp)
target_link_libraries(06_counter_app DeclarativeUI Qt6::Core Qt6::Widgets)

# **Set output directory**
set_target_properties(
    01_hello_world
    02_json_ui_loading
    03_simple_state
    04_basic_hot_reload
    05_event_handling
    06_counter_app
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/01_basic
)

# **Copy resources**
add_custom_target(CopyBasicExampleResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/examples/01_basic/resources
)

# **Dependencies**
add_dependencies(02_json_ui_loading CopyBasicExampleResources)
add_dependencies(04_basic_hot_reload CopyBasicExampleResources)
add_dependencies(05_event_handling CopyBasicExampleResources)
add_dependencies(06_counter_app CopyBasicExampleResources)
