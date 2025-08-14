set(SOURCES
    AnimationEngine.cpp
)

add_library(Animation ${SOURCES} AnimationEngine.hpp)

target_link_libraries(Animation
    Qt6::Core
    Qt6::Widgets
)

target_include_directories(Animation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(Animation PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)
