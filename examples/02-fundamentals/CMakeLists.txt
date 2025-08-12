# **Fundamentals Examples**
# Core concepts and fundamental features

# **Event Handling Example**
# Basic event handling and signal-slot connections
add_executable(EventHandlingExample event-handling/event-handling.cpp)
target_include_directories(EventHandlingExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(EventHandlingExample 
    DeclarativeUI 
    Components 
    Qt6::Core 
    Qt6::Widgets
)
set_target_properties(EventHandlingExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/02-fundamentals
)

# **JSON UI Loading Example**
# Loading UI definitions from JSON files
add_executable(JSONUILoadingExample json-ui-loading/json-ui-loading.cpp)
target_include_directories(JSONUILoadingExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(JSONUILoadingExample 
    DeclarativeUI 
    Components 
    Qt6::Core 
    Qt6::Widgets
)
set_target_properties(JSONUILoadingExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/02-fundamentals
)

# **Property Binding Example**
# Data binding and property synchronization
add_executable(PropertyBindingExample property-binding/property-binding.cpp)
target_include_directories(PropertyBindingExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(PropertyBindingExample 
    DeclarativeUI 
    Components 
    Qt6::Core 
    Qt6::Widgets
)
set_target_properties(PropertyBindingExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/02-fundamentals
)

# **State Management Example**
# Application state management patterns
add_executable(StateManagementExample state-management/state-management.cpp)
target_include_directories(StateManagementExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(StateManagementExample 
    DeclarativeUI 
    Components 
    Qt6::Core 
    Qt6::Widgets
)
set_target_properties(StateManagementExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/02-fundamentals
)

message(STATUS "Fundamentals examples configured")
