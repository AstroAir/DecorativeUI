# **Components Library**
add_library(Components
    # Button Components
    Button.hpp
    Button.cpp
    RadioButton.hpp
    RadioButton.cpp
    ToolButton.hpp
    ToolButton.cpp

    # Input Components
    LineEdit.hpp
    LineEdit.cpp
    CheckBox.hpp
    CheckBox.cpp
    ComboBox.hpp
    ComboBox.cpp
    SpinBox.hpp
    SpinBox.cpp
    DoubleSpinBox.hpp
    DoubleSpinBox.cpp
    Slider.hpp
    Slider.cpp

    # Display Components
    Label.hpp
    Label.cpp
    ProgressBar.hpp
    ProgressBar.cpp
    TextEdit.hpp
    TextEdit.cpp
    PlainTextEdit.hpp
    PlainTextEdit.cpp
    LCDNumber.hpp
    LCDNumber.cpp

    # Container Components
    Container.hpp
    Container.cpp
    Layout.hpp
    Layout.cpp
    GroupBox.hpp
    TabWidget.hpp
    Frame.hpp
    Frame.cpp
    ScrollArea.hpp
    ScrollArea.cpp
    Splitter.hpp
    Splitter.cpp
    Widget.hpp
    Widget.cpp

    # Advanced Components
    TableView.hpp
    TableView.cpp
    TreeView.hpp
    TreeView.cpp
    ListView.hpp
    ListView.cpp
    MenuBar.hpp
    DateTimeEdit.hpp
    Calendar.hpp

    # Dialogs
    FileDialog.hpp
    FileDialog.cpp
    ColorDialog.hpp
    ColorDialog.cpp
    FontDialog.hpp
    FontDialog.cpp
    MessageBox.hpp
    MessageBox.cpp

    # Other Components
    ToolBar.hpp
    ToolBar.cpp
    StatusBar.hpp
    StatusBar.cpp
    DockWidget.hpp
    DockWidget.cpp
    Dial.hpp
    Dial.cpp
)

target_link_libraries(Components
    Core
    Qt6::Core
    Qt6::Widgets
)

target_include_directories(Components PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../Exceptions
)

set_target_properties(Components PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)
