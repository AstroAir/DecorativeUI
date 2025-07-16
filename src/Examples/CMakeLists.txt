# Enable Qt MOC processing
set(CMAKE_AUTOMOC ON)

add_executable(main main.cpp)
target_link_libraries(main DeclarativeUI Qt6::Core Qt6::Widgets)

add_executable(hot_reload_example hot_reload_example.cpp)
target_link_libraries(hot_reload_example DeclarativeUI Qt6::Core Qt6::Widgets)

# **New examples with enhanced component support**
add_executable(form_example form_example.cpp)
target_link_libraries(form_example DeclarativeUI Qt6::Core Qt6::Widgets)

add_executable(dashboard_example dashboard_example.cpp)
target_link_libraries(dashboard_example DeclarativeUI Qt6::Core Qt6::Widgets)

add_executable(settings_example settings_example.cpp)
target_link_libraries(settings_example DeclarativeUI Qt6::Core Qt6::Widgets)

# **Comprehensive example showcasing all new components**
add_executable(comprehensive_example comprehensive_example.cpp)
target_link_libraries(comprehensive_example DeclarativeUI Components Qt6::Core Qt6::Widgets)
