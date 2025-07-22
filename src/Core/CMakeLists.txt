set(SOURCES
    UIElement.cpp
    DeclarativeBuilder.cpp
)

add_library(Core ${SOURCES} UIElement.hpp DeclarativeBuilder.hpp)

target_link_libraries(Core
    Qt6::Core
    Qt6::Widgets
)

target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(Core PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)
