# **Command System Examples (Experimental)**
# Command architecture demonstrations
# Note: Some examples may be disabled due to API mismatches

# **Command Basics Example**
# Introduction to command system
add_executable(CommandBasicsExample command-basics/command-basics.cpp)
target_include_directories(CommandBasicsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandBasicsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(CommandBasicsExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05-command-system
)

# **Command Basics Simple Example**
# Simplified command basics
add_executable(CommandBasicsSimpleExample command-basics/command-basics-simple.cpp)
target_include_directories(CommandBasicsSimpleExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandBasicsSimpleExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(CommandBasicsSimpleExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05-command-system
)

# **Command Builder Example**
# Command Builder patterns
add_executable(CommandBuilderExample command-builder/command-builder.cpp)
target_include_directories(CommandBuilderExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandBuilderExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(CommandBuilderExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05-command-system
)

# **Command UI Example**
# Command-based UI construction
add_executable(CommandUIExample command-builder/CommandUIExample.cpp)
target_include_directories(CommandUIExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(CommandUIExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(CommandUIExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05-command-system
)

# **JSON Commands Example**
# Loading UI from JSON with Commands
add_executable(JSONCommandsExample json-commands/json-commands.cpp)
target_include_directories(JSONCommandsExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(JSONCommandsExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(JSONCommandsExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05-command-system
)

# **State Integration Example**
# State management with Commands
add_executable(StateIntegrationExample state-integration/state-integration.cpp)
target_include_directories(StateIntegrationExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(StateIntegrationExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)
set_target_properties(StateIntegrationExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/05-command-system
)

message(STATUS "Command System examples configured successfully")

