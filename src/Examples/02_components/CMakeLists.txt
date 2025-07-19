# **Component Examples (06-15)**
# Comprehensive component usage and showcase examples

# **06 - Basic Components** (Available)
add_executable(06_basic_components 06_basic_components.cpp)
target_link_libraries(06_basic_components DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **07 - Input Components** (Available)
add_executable(07_input_components 07_input_components.cpp)
target_link_libraries(07_input_components DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **08 - Comprehensive Components**
add_executable(08_comprehensive_components 08_comprehensive_components.cpp)
target_link_libraries(08_comprehensive_components DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **09 - Enhanced Components**
add_executable(09_enhanced_components 09_enhanced_components.cpp)
target_link_libraries(09_enhanced_components DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **10 - New Components**
add_executable(10_new_components 10_new_components.cpp)
target_link_libraries(10_new_components DeclarativeUI Components Qt6::Core Qt6::Widgets)

# **TODO: Additional component examples to be implemented**
# 11 - Dialog Components (MessageBox, FileDialog, ColorDialog)
# 12 - Container Components (Splitter, ScrollArea, DockWidget)
# 13 - Menu Components (MenuBar, ToolBar, StatusBar)
# 14 - Custom Components (Creating and registering custom components)
# 15 - Advanced Layout Examples

# **Set output directory**
set_target_properties(
    06_basic_components
    07_input_components
    08_comprehensive_components
    09_enhanced_components
    10_new_components
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/02_components
)

# **Copy resources**
add_custom_target(CopyComponentExampleResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/examples/02_components/resources
)

# **Dependencies**
add_dependencies(06_basic_components CopyComponentExampleResources)
add_dependencies(07_input_components CopyComponentExampleResources)
add_dependencies(08_comprehensive_components CopyComponentExampleResources)
add_dependencies(09_enhanced_components CopyComponentExampleResources)
add_dependencies(10_new_components CopyComponentExampleResources)
