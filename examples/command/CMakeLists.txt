# **Command System Examples Configuration**
# Examples demonstrating command system functionality

# **Set output directory for command examples**
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/command)

# **Command UI Example** (temporarily disabled due to API mismatches)
# add_executable(CommandUIExample CommandUIExample.cpp CommandUIExample.hpp)
# target_include_directories(CommandUIExample PRIVATE
#     ${CMAKE_SOURCE_DIR}/src
# )
# target_link_libraries(CommandUIExample
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
# )

# **Integration Example** (temporarily disabled due to API mismatches)
# add_executable(IntegrationExample IntegrationExample.cpp IntegrationExample.hpp)
# target_include_directories(IntegrationExample PRIVATE
#     ${CMAKE_SOURCE_DIR}/src
# )
# target_link_libraries(IntegrationExample
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
# )

# **Command Builder Example** (temporarily disabled due to API mismatches)
# add_executable(CommandBuilderExample command_builder_example.cpp)
# target_include_directories(CommandBuilderExample PRIVATE
#     ${CMAKE_SOURCE_DIR}/src
# )
# target_link_libraries(CommandBuilderExample
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
# )

# **JSON Command Example** (temporarily disabled due to API mismatches)
# add_executable(JSONCommandExample json_command_example.cpp)
# target_include_directories(JSONCommandExample PRIVATE
#     ${CMAKE_SOURCE_DIR}/src
# )
# target_link_libraries(JSONCommandExample
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
# )

# **State Integration Example** (temporarily disabled due to API mismatches)
# add_executable(StateIntegrationExample state_integration_example.cpp)
# target_include_directories(StateIntegrationExample PRIVATE
#     ${CMAKE_SOURCE_DIR}/src
# )
# target_link_libraries(StateIntegrationExample
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
# )

# **Main Command Example** (temporarily disabled due to API mismatches)
# add_executable(CommandMainExample main.cpp)
# target_include_directories(CommandMainExample PRIVATE
#     ${CMAKE_SOURCE_DIR}/src
# )
# target_link_libraries(CommandMainExample
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
# )

# **Integration Main Example** (temporarily disabled due to API mismatches)
# add_executable(IntegrationMainExample integration_main.cpp)
# target_include_directories(IntegrationMainExample PRIVATE
#     ${CMAKE_SOURCE_DIR}/src
# )
# target_link_libraries(IntegrationMainExample
#     DeclarativeUI
#     Components
#     Qt6::Core
#     Qt6::Widgets
# )

# **Make examples depend on resources**
# add_dependencies(CommandUIExample CopyExampleResources)
# add_dependencies(IntegrationExample CopyExampleResources)
# add_dependencies(CommandBuilderExample CopyExampleResources)
# add_dependencies(JSONCommandExample CopyExampleResources)
# add_dependencies(StateIntegrationExample CopyExampleResources)
# add_dependencies(CommandMainExample CopyExampleResources)
# add_dependencies(IntegrationMainExample CopyExampleResources)
