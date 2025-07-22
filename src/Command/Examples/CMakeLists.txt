# **Command System Examples**

# **Find required packages**
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# **Command UI Example**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CommandUIExample.hpp)
    add_executable(CommandUIExample
        CommandUIExample.cpp
        main.cpp
    )
    
    target_link_libraries(CommandUIExample
        DeclarativeUI
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
    )
    
    target_include_directories(CommandUIExample PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    
    set_target_properties(CommandUIExample PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
    )
    
    # **Copy to build directory**
    set_target_properties(CommandUIExample PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )
    
    message(STATUS "Command UI Example will be built")
endif()

# **Integration Example**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/IntegrationExample.hpp)
    add_executable(IntegrationExample
        IntegrationExample.cpp
        integration_main.cpp
    )
    
    target_link_libraries(IntegrationExample
        DeclarativeUI
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
    )
    
    target_include_directories(IntegrationExample PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    
    set_target_properties(IntegrationExample PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
    )
    
    # **Copy to build directory**
    set_target_properties(IntegrationExample PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )
    
    message(STATUS "Integration Example will be built")
endif()

# **Command Builder Example**
add_executable(CommandBuilderExample
    command_builder_example.cpp
)

target_link_libraries(CommandBuilderExample
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

target_include_directories(CommandBuilderExample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(CommandBuilderExample PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# **State Integration Example**
add_executable(StateIntegrationExample
    state_integration_example.cpp
)

target_link_libraries(StateIntegrationExample
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

target_include_directories(StateIntegrationExample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(StateIntegrationExample PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# **JSON Command Loading Example**
add_executable(JSONCommandExample
    json_command_example.cpp
)

target_link_libraries(JSONCommandExample
    DeclarativeUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

target_include_directories(JSONCommandExample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

set_target_properties(JSONCommandExample PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# **Copy example JSON files**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/example_ui.json)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/example_ui.json
        ${CMAKE_BINARY_DIR}/examples/example_ui.json
        COPYONLY
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/mixed_components.json)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/mixed_components.json
        ${CMAKE_BINARY_DIR}/examples/mixed_components.json
        COPYONLY
    )
endif()

message(STATUS "Command system examples configured")
