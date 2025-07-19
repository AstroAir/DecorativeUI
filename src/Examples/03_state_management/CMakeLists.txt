# **State Management Examples (16-20)**
# Advanced state management patterns and techniques

# **16 - Reactive State**
add_executable(16_reactive_state 16_reactive_state.cpp)
target_link_libraries(16_reactive_state DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **TODO: Additional state management examples to be implemented**
# 17 - Computed State (Computed properties and derived state)
# 18 - State Validation (State validation and error handling)
# 19 - State History (Undo/redo functionality)
# 20 - State Persistence (Saving and loading state)

# **Set output directory**
set_target_properties(
    16_reactive_state
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/03_state_management
)

# **Copy resources (only if directory exists)**
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    add_custom_target(CopyStateExampleResources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/examples/03_state_management/resources
    )

    # **Dependencies**
    add_dependencies(16_reactive_state CopyStateExampleResources)
endif()
