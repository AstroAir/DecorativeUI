add_library(Components
    Button.hpp
    Button.cpp
    Label.hpp
    LineEdit.hpp
    LineEdit.cpp
    ComboBox.hpp
    ComboBox.cpp
    CheckBox.hpp
    CheckBox.cpp
    Slider.hpp
    Slider.cpp
    ProgressBar.hpp
    ProgressBar.cpp
    SpinBox.hpp
    TextEdit.hpp
    TableView.hpp
    TreeView.hpp
    Layout.hpp
    Container.hpp
)

target_link_libraries(Components
    Qt6::Core
    Qt6::Widgets
)

target_include_directories(Components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(Components PROPERTIES
    AUTOMOC ON
)
