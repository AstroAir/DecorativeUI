# **Getting Started Examples**
# Perfect for absolute beginners to DeclarativeUI

# **Hello World Example**
# Minimal window demonstration
add_executable(HelloWorldExample hello-world/hello-world.cpp)
target_include_directories(HelloWorldExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(HelloWorldExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(HelloWorldExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/01-getting-started
)

# **Basic UI Example**
# Introduction to basic UI components
add_executable(BasicUIExample basic-ui/basic-ui.cpp)
target_include_directories(BasicUIExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(BasicUIExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(BasicUIExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/01-getting-started
)

# **Simple Events Example**
# Basic event handling patterns
add_executable(SimpleEventsExample simple-events/simple-events.cpp)
target_include_directories(SimpleEventsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(SimpleEventsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(SimpleEventsExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/01-getting-started
)

message(STATUS "Getting Started examples configured")
