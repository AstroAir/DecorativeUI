set(SOURCES
    UIElement.cpp
    DeclarativeBuilder.cpp
)

add_library(Core ${SOURCES} UIElement.hpp DeclarativeBuilder.hpp)
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
