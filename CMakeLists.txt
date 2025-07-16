cmake_minimum_required(VERSION 3.20)
project(DeclarativeUI VERSION 1.0.0 LANGUAGES CXX)

# **Modern C++ standard**
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# **Find Qt6 with required components**
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Test)

# **Compiler-specific optimizations**
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# **Debug/Release configurations**
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DECLARATIVE_UI_DEBUG)
endif()

# **Create main library**
add_library(DeclarativeUI STATIC
    # Core
    src/Core/UIElement.cpp
    src/Core/DeclarativeBuilder.cpp
    
    # JSON Support
    src/JSON/JSONUILoader.cpp
    src/JSON/ComponentRegistry.cpp
    
    # Hot Reload
    src/HotReload/FileWatcher.cpp
    src/HotReload/HotReloadManager.cpp
    src/HotReload/PerformanceMonitor.cpp
    
    # Binding
    src/Binding/StateManager.cpp
    
    # Command System
    src/Command/CommandSystem.cpp
    src/Command/BuiltinCommands.cpp
    src/Command/CommandIntegration.cpp
)

target_link_libraries(DeclarativeUI 
    Components
    Qt6::Core 
    Qt6::Widgets
    Qt6::Network
)

target_include_directories(DeclarativeUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# **Example applications**
add_executable(CounterApp src/Examples/main.cpp)
target_link_libraries(CounterApp DeclarativeUI Qt6::Core Qt6::Widgets)

add_executable(HotReloadDemo src/Examples/hot_reload_example.cpp)
target_link_libraries(HotReloadDemo DeclarativeUI Qt6::Core Qt6::Widgets)

add_executable(CommandDemo src/Examples/advanced_command_demo.cpp)
target_link_libraries(CommandDemo DeclarativeUI Qt6::Core Qt6::Widgets)

# **Test applications**
add_executable(CommandSystemTest src/Tests/test_command_system.cpp)
target_link_libraries(CommandSystemTest DeclarativeUI Qt6::Core Qt6::Widgets Qt6::Test)

# **Enable Qt MOC**
set_target_properties(DeclarativeUI CounterApp HotReloadDemo CommandDemo CommandSystemTest PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)

# **Copy resources to build directory**
add_custom_target(CopyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_BINARY_DIR}/Resources
)

add_dependencies(HotReloadDemo CopyResources)

# **Add Components subdirectory**
add_subdirectory(src/Components)

# **Add Examples subdirectory**
add_subdirectory(src/Examples)

# **Install targets**
install(TARGETS DeclarativeUI CounterApp HotReloadDemo CommandDemo CommandSystemTest
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY Resources/ DESTINATION share/DeclarativeUI/Resources)
