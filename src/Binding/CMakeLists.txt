add_library(Binding STATIC
    PropertyBinding.cpp
    StateManager.cpp
)

target_include_directories(Binding PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Binding
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(Binding PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)
