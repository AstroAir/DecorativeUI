# Advanced Components Examples
# Demonstrates sophisticated composite components

cmake_minimum_required(VERSION 3.16)

# **Advanced Components Showcase**
# Comprehensive demonstration of all advanced components
add_executable(AdvancedComponentsShowcase 
    showcase/main.cpp
    showcase/AdvancedComponentsShowcase.cpp
    showcase/AdvancedComponentsShowcase.hpp
)

target_include_directories(AdvancedComponentsShowcase PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(AdvancedComponentsShowcase
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(AdvancedComponentsShowcase PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

# **SearchBox Example**
# Demonstrates advanced search functionality
add_executable(SearchBoxExample 
    searchbox/main.cpp
    searchbox/SearchBoxExample.cpp
    searchbox/SearchBoxExample.hpp
)

target_include_directories(SearchBoxExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(SearchBoxExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(SearchBoxExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

# **DataTable Example**
# Demonstrates advanced data grid functionality
add_executable(DataTableExample 
    datatable/main.cpp
    datatable/DataTableExample.cpp
    datatable/DataTableExample.hpp
)

target_include_directories(DataTableExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(DataTableExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(DataTableExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

# **PropertyEditor Example**
# Demonstrates dynamic property editing
add_executable(PropertyEditorExample 
    propertyeditor/main.cpp
    propertyeditor/PropertyEditorExample.cpp
    propertyeditor/PropertyEditorExample.hpp
)

target_include_directories(PropertyEditorExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(PropertyEditorExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(PropertyEditorExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

# **FormBuilder Example**
# Demonstrates dynamic form generation
add_executable(FormBuilderExample 
    formbuilder/main.cpp
    formbuilder/FormBuilderExample.cpp
    formbuilder/FormBuilderExample.hpp
)

target_include_directories(FormBuilderExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(FormBuilderExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(FormBuilderExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

# **FileExplorer Example**
# Demonstrates file system browser
add_executable(FileExplorerExample 
    fileexplorer/main.cpp
    fileexplorer/FileExplorerExample.cpp
    fileexplorer/FileExplorerExample.hpp
)

target_include_directories(FileExplorerExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(FileExplorerExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(FileExplorerExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

# **ChartWidget Example**
# Demonstrates charting capabilities
add_executable(ChartWidgetExample 
    chartwidget/main.cpp
    chartwidget/ChartWidgetExample.cpp
    chartwidget/ChartWidgetExample.hpp
)

target_include_directories(ChartWidgetExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ChartWidgetExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(ChartWidgetExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

# **ColorPicker Example**
# Demonstrates advanced color selection
add_executable(ColorPickerExample 
    colorpicker/main.cpp
    colorpicker/ColorPickerExample.cpp
    colorpicker/ColorPickerExample.hpp
)

target_include_directories(ColorPickerExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ColorPickerExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(ColorPickerExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

# **DateTimePicker Example**
# Demonstrates advanced date/time selection
add_executable(DateTimePickerExample 
    datetimepicker/main.cpp
    datetimepicker/DateTimePickerExample.cpp
    datetimepicker/DateTimePickerExample.hpp
)

target_include_directories(DateTimePickerExample PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(DateTimePickerExample
    DeclarativeUI
    Components
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(DateTimePickerExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples/04-advanced-components
)

message(STATUS "Advanced Components examples configured successfully")
