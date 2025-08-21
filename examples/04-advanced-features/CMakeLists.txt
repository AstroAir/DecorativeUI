# **Advanced Features Examples**
# Advanced DeclarativeUI features and patterns

# **Basic Hot Reload Example**
# Introduction to hot reload functionality
add_executable(BasicHotReloadExample hot-reload/basic-hot-reload.cpp)
target_include_directories(BasicHotReloadExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(BasicHotReloadExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(BasicHotReloadExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-features
)

# **Advanced Hot Reload Example**
# Advanced hot reload features and patterns
add_executable(AdvancedHotReloadExample hot-reload/advanced-hot-reload.cpp)
target_include_directories(AdvancedHotReloadExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(AdvancedHotReloadExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(AdvancedHotReloadExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-features
)

# **Reactive State Example**
# Reactive state management patterns
add_executable(ReactiveStateExample reactive-state/reactive-state.cpp)
target_include_directories(ReactiveStateExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(ReactiveStateExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(ReactiveStateExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-features
)

# **File Watching Example**
# File watching and dynamic updates
add_executable(FileWatchingExample file-watching/file-watching.cpp)
target_include_directories(FileWatchingExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(FileWatchingExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
set_target_properties(FileWatchingExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-features
)

# **Performance Example**
# Performance monitoring and optimization
add_executable(PerformanceExample performance/performance.cpp)
target_include_directories(PerformanceExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(PerformanceExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(PerformanceExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-features
)

message(STATUS "Advanced Features examples configured")
