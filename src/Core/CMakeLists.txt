set(SOURCES
    UIElement.cpp
    DeclarativeBuilder.cpp
    ErrorHandling.cpp
    Lifecycle.cpp
    ErrorBoundary.cpp
    ConditionalRendering.cpp
)

add_library(Core ${SOURCES} UIElement.hpp DeclarativeBuilder.hpp Lifecycle.hpp ErrorBoundary.hpp ConditionalRendering.hpp)

target_link_libraries(Core
    Animation
    Binding
    Qt6::Core
    Qt6::Widgets
)

target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Animation
)

set_target_properties(Core PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)
