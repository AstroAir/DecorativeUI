# Command System

The Command System provides a comprehensive framework for handling UI interactions and operations in DeclarativeUI. It implements the Command Pattern with modern C++ features and Qt integration.

## Overview

The Command System consists of several key components:

- **Core Command Infrastructure**: Base command classes and execution framework
- **Component-Specific Commands**: Specialized commands for each UI component type
- **Widget Mapping**: Automatic mapping between commands and Qt widgets
- **Built-in Commands**: Common operations like file handling, clipboard, messaging
- **Adapters**: Integration with other DeclarativeUI systems

## Architecture

### Core Components

- `CommandSystem.hpp/cpp` - Core command infrastructure and base classes
- `CommandContext.hpp/cpp` - Command execution context and parameter management
- `CommandFactory.hpp/cpp` - Command registration and creation
- `CommandBuilder.hpp/cpp` - Fluent API for building complex commands
- `BuiltinCommands.hpp/cpp` - Built-in command implementations

### Component Commands

The system now includes specialized commands for all UI components:

#### Button Components

- `ButtonCommand` - Handle button clicks, text changes, enable/disable
- `CheckBoxCommand` - Toggle states, check/uncheck with undo support
- `RadioButtonCommand` - Selection with undo support

#### Text Components

- `LabelCommand` - Text setting and clearing with undo
- `LineEditCommand` - Text editing, clearing, selection with undo
- `TextEditCommand` - Rich text editing, appending, clearing with undo

#### Input Components

- `SpinBoxCommand` - Value setting, stepping, range configuration with undo
- `DoubleSpinBoxCommand` - Decimal value setting, stepping, decimals configuration with undo
- `SliderCommand` - Value setting, range configuration with undo
- `DialCommand` - Value setting, range configuration, notches visibility with undo
- `ComboBoxCommand` - Item selection, text setting, item management with undo
- `DateTimeEditCommand` - DateTime setting, date/time ranges, display format with undo
- `ProgressBarCommand` - Value setting, range configuration, text visibility, reset

#### Display Components

- `LCDNumberCommand` - Value display, digit count, mode, segment style
- `CalendarCommand` - Date selection, date ranges, grid visibility with undo

#### View Components

- `ListViewCommand` - Model operations, item selection, item management with undo
- `TableViewCommand` - Model operations, cell/row/column selection, data setting with undo
- `TreeViewCommand` - Model operations, item selection, expand/collapse, data setting with undo

#### Container Components

- `TabWidgetCommand` - Tab switching, text setting with undo
- `GroupBoxCommand` - Title setting, checkable state, checked state with undo
- `FrameCommand` - Frame style, line width, mid line width
- `ScrollAreaCommand` - Widget resizable, scroll bar policies
- `SplitterCommand` - Sizes setting, orientation, children collapsible with undo
- `DockWidgetCommand` - Floating state, allowed areas, features with undo

#### Menu/Toolbar Components

- `MenuBarCommand` - Menu management, native menu bar setting
- `StatusBarCommand` - Message display, widget management
- `ToolBarCommand` - Action management, orientation, movable state with undo
- `ToolButtonCommand` - Default action, popup mode, button style with undo

### Widget Mapping

The `WidgetMapper` provides automatic mapping between commands and Qt widgets:

- **Factory Functions**: Create appropriate Qt widgets for each command type
- **Property Synchronization**: Bidirectional sync between command and widget properties
- **Event Mapping**: Automatic forwarding of Qt signals to command events
- **Custom Setup**: Component-specific initialization and configuration

### Supported Widget Mappings

The system now supports comprehensive mappings for:

- **Button widgets**: QPushButton, QCheckBox, QRadioButton, QToolButton
- **Text widgets**: QLabel, QLineEdit, QTextEdit, QPlainTextEdit
- **Input widgets**: QSpinBox, QDoubleSpinBox, QSlider, QDial, QComboBox, QDateTimeEdit
- **Display widgets**: QProgressBar, QLCDNumber, QCalendarWidget
- **Container widgets**: QTabWidget, QGroupBox, QScrollArea, QSplitter, QFrame, QDockWidget
- **View widgets**: QListView, QTableView, QTreeView
- **Menu/Toolbar widgets**: QMenuBar, QStatusBar, QToolBar

## Usage

### Basic Command Usage

```cpp
// Create a command context
CommandContext context;
context.setParameter("widget", QString("myButton"));
context.setParameter("operation", QString("setText"));
context.setParameter("text", QString("Click Me!"));

// Execute a button command
ButtonCommand command(context);
auto result = command.execute(context);

if (result.isSuccess()) {
    qDebug() << "Command executed successfully";
}
```

### Using the Command Factory

```cpp
// Register and use commands through the factory
auto& factory = CommandFactory::instance();

// Execute a command by name
CommandContext context;
context.setParameter("widget", QString("myCheckBox"));
context.setParameter("operation", QString("toggle"));

auto result = factory.execute("checkbox", context);
```

### Widget Mapping

```cpp
// Get the widget mapper instance
auto& mapper = WidgetMapper::instance();

// Create a widget for a command
auto widget = mapper.createWidget(myCommand);

// Establish bidirectional binding
mapper.establishBinding(myCommand, widget.get());
```

## Command Operations

### Button Commands

- `click` - Trigger button click
- `setText` - Set button text
- `setEnabled` - Enable/disable button

### CheckBox Commands

- `toggle` - Toggle checked state
- `setChecked` - Set specific checked state

### RadioButton Commands

- `select` - Select radio button
- `setChecked` - Set specific checked state

### Text Commands

- `setText` - Set text content
- `append` - Append text (TextEdit only)
- `clear` - Clear all text
- `selectAll` - Select all text (LineEdit only)

### Input Commands

- `setValue` - Set numeric value
- `stepUp`/`stepDown` - Step values (SpinBox, DoubleSpinBox)
- `setRange` - Set min/max range
- `setDecimals` - Set decimal places (DoubleSpinBox)
- `setNotchesVisible` - Set notches visibility (Dial)
- `setCurrentIndex` - Set selection (ComboBox)
- `addItem` - Add new item (ComboBox)

### DateTime Commands

- `setDateTime` - Set date and time
- `setDateRange` - Set minimum and maximum dates
- `setTimeRange` - Set minimum and maximum times
- `setDisplayFormat` - Set display format string

### Progress Commands

- `setValue` - Set progress value
- `setRange` - Set progress range
- `setTextVisible` - Set text visibility
- `reset` - Reset to minimum value

### Display Commands

- `display` - Display value (LCDNumber)
- `setDigitCount` - Set number of digits (LCDNumber)
- `setMode` - Set display mode (LCDNumber)
- `setSegmentStyle` - Set segment style (LCDNumber)
- `setSelectedDate` - Set selected date (Calendar)
- `setGridVisible` - Set grid visibility (Calendar)

### View Commands

- `selectItem` - Select item by index
- `selectCell` - Select table cell (TableView)
- `selectRow` - Select table row (TableView)
- `selectColumn` - Select table column (TableView)
- `addItem` - Add new item (ListView)
- `removeItem` - Remove item by index
- `clearSelection` - Clear current selection
- `expandItem` - Expand tree item (TreeView)
- `collapseItem` - Collapse tree item (TreeView)
- `setItemData` - Set item data (TableView, TreeView)

### Container Commands

- `setCurrentIndex` - Switch tabs (TabWidget)
- `setTabText` - Set tab text (TabWidget)
- `setTitle` - Set title (GroupBox)
- `setCheckable` - Set checkable state (GroupBox)
- `setChecked` - Set checked state (GroupBox)
- `setFrameStyle` - Set frame style (Frame)
- `setLineWidth` - Set line width (Frame)
- `setMidLineWidth` - Set mid line width (Frame)
- `setWidgetResizable` - Set widget resizable (ScrollArea)
- `setScrollBarPolicy` - Set scroll bar policies (ScrollArea)
- `setSizes` - Set splitter sizes (Splitter)
- `setOrientation` - Set orientation (Splitter)
- `setChildrenCollapsible` - Set children collapsible (Splitter)
- `setFloating` - Set floating state (DockWidget)
- `setAllowedAreas` - Set allowed dock areas (DockWidget)
- `setFeatures` - Set dock widget features (DockWidget)

### Menu/Toolbar Commands

- `addMenu` - Add menu (MenuBar)
- `removeMenu` - Remove menu (MenuBar)
- `setNativeMenuBar` - Set native menu bar (MenuBar)
- `showMessage` - Show status message (StatusBar)
- `clearMessage` - Clear status message (StatusBar)
- `addWidget` - Add widget to status bar (StatusBar)
- `removeWidget` - Remove widget from status bar (StatusBar)
- `addAction` - Add action (ToolBar)
- `removeAction` - Remove action (ToolBar)
- `setOrientation` - Set toolbar orientation (ToolBar)
- `setMovable` - Set movable state (ToolBar)
- `setDefaultAction` - Set default action (ToolButton)
- `setPopupMode` - Set popup mode (ToolButton)
- `setToolButtonStyle` - Set button style (ToolButton)

## Undo/Redo Support

Most component commands support undo operations:

```cpp
// Execute a command
auto result = command.execute(context);

// Check if undo is supported
if (command.canUndo(context)) {
    // Undo the command
    auto undoResult = command.undo(context);
}
```

## Integration

### State Management

Commands integrate with the DeclarativeUI state management system through adapters.

### Event System

Commands can emit and handle events through the CommandEvents system.

### JSON Configuration

Commands can be configured and loaded from JSON through the JSONCommandLoader adapter.

## Examples

Examples demonstrating command usage can be found in:

- `examples/05-command-system/command-basics/`
- `examples/05-command-system/command-builder/`
- `examples/05-command-system/component-commands/`

## Testing

Comprehensive tests are available in:

- `tests/command/test_component_commands.cpp`
- `tests/command/test_command_*.cpp`

## Files Structure

```
src/Command/
├── README.md                           # This file
├── CommandSystem.hpp/cpp               # Core command infrastructure
├── ComponentCommands.hpp/cpp           # Component-specific commands (core)
├── ComponentCommandsExtended.cpp       # Additional component commands
├── ComponentCommandsInput.cpp          # Input/Form components (DoubleSpinBox, Dial, DateTimeEdit, ProgressBar)
├── ComponentCommandsDisplay.cpp        # Display components (LCDNumber, Calendar)
├── ComponentCommandsView.cpp           # View/Model components (ListView, TableView, TreeView)
├── ComponentCommandsContainer.cpp      # Container/Layout components (GroupBox, Frame, ScrollArea)
├── ComponentCommandsContainerExtended.cpp # Extended container components (Splitter, DockWidget)
├── ComponentCommandsMenu.cpp           # Menu/Toolbar components (MenuBar, StatusBar)
├── ComponentCommandsMenuExtended.cpp   # Extended menu components (ToolBar, ToolButton)
├── BuiltinCommands.hpp/cpp             # Built-in commands
├── WidgetMapper.hpp/cpp                # Widget mapping system
├── CommandBuilder.hpp/cpp              # Command builder API
├── CommandFactory.hpp/cpp              # Command factory
├── CommandContext.hpp/cpp              # Execution context
├── UICommand.hpp/cpp                   # UI command base classes
├── UICommandFactory.hpp/cpp            # UI command factory
├── CoreCommands.hpp/cpp                # Core command implementations
├── SpecializedCommands.hpp/cpp         # Specialized commands
├── CommandBinding.hpp/cpp              # Command binding system
├── CommandEvents.hpp/cpp               # Event handling
├── CommandIntegration.hpp/cpp          # System integration
├── MVCIntegration.hpp/cpp              # MVC pattern integration
└── Adapters/                           # Integration adapters
    ├── ComponentSystemAdapter.*
    ├── IntegrationManager.*
    ├── JSONCommandLoader.*
    ├── StateManagerAdapter.*
    └── UIElementAdapter.*
```
