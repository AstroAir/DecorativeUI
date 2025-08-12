# Components Library API Reference

The Components library provides a comprehensive set of UI components built on top of the DeclarativeUI Core framework. All components inherit from `UIElement` and provide fluent interfaces for easy configuration.

## Component Categories

### Input Components

- **Button** - Push button with click handling
- **LineEdit** - Single-line text input
- **CheckBox** - Checkbox with tri-state support
- **ComboBox** - Dropdown selection
- **SpinBox** - Numeric input with spin controls
- **Slider** - Value selection slider

### Display Components

- **Label** - Text and image display
- **ProgressBar** - Progress indication
- **TextEdit** - Multi-line text display and editing

### Container Components

- **Container** - Generic container widget
- **Layout** - Layout management
- **GroupBox** - Grouped controls with border
- **TabWidget** - Tabbed interface

### Advanced Components

- **TableView** - Tabular data display
- **TreeView** - Hierarchical data display
- **MenuBar** - Application menu bar
- **DateTimeEdit** - Date and time input

## Input Components

### Button Class

A customizable push button component.

#### Header

```cpp
#include "Components/Button.hpp"
```

#### Class Declaration

```cpp
namespace DeclarativeUI::Components {
    class Button : public Core::UIElement {
        Q_OBJECT
    public:
        explicit Button(QObject* parent = nullptr);

        // Fluent interface methods...
    };
}
```

#### Methods

##### `text(const QString& text) -> Button&`

Sets the button text.

##### `icon(const QIcon& icon) -> Button&`

Sets the button icon.

##### `onClick(std::function<void()> handler) -> Button&`

Sets the click event handler.

##### `enabled(bool enabled) -> Button&`

Enables or disables the button.

##### `style(const QString& stylesheet) -> Button&`

Sets custom stylesheet.

#### Usage Example

```cpp
auto button = std::make_unique<Components::Button>();
button->text("Click Me")
      .icon(QIcon(":/icons/button.png"))
      .onClick([]() {
          qDebug() << "Button clicked!";
      })
      .style("QPushButton { background-color: #007bff; color: white; }");
button->initialize();
```

### LineEdit Class

Single-line text input component.

#### Methods

##### `text(const QString& text) -> LineEdit&`

Sets the initial text.

##### `placeholder(const QString& placeholder) -> LineEdit&`

Sets placeholder text.

##### `readOnly(bool readonly) -> LineEdit&`

Sets read-only mode.

##### `maxLength(int length) -> LineEdit&`

Sets maximum text length.

##### `validator(QValidator* validator) -> LineEdit&`

Sets input validator.

##### `echoMode(QLineEdit::EchoMode mode) -> LineEdit&`

Sets echo mode (normal, password, etc.).

##### `onTextChanged(std::function<void(const QString&)> handler) -> LineEdit&`

Sets text change handler.

##### `onTextEdited(std::function<void(const QString&)> handler) -> LineEdit&`

Sets text edit handler.

##### `onReturnPressed(std::function<void()> handler) -> LineEdit&`

Sets return key handler.

#### Usage Example

```cpp
auto lineEdit = std::make_unique<Components::LineEdit>();
lineEdit->placeholder("Enter your name...")
        .maxLength(50)
        .onTextChanged([](const QString& text) {
            qDebug() << "Text changed:" << text;
        })
        .onReturnPressed([]() {
            qDebug() << "Return pressed!";
        });
lineEdit->initialize();
```

### CheckBox Class

Checkbox component with tri-state support.

#### Methods

##### `text(const QString& text) -> CheckBox&`

Sets the checkbox label text.

##### `checked(bool checked) -> CheckBox&`

Sets the checked state.

##### `tristate(bool tristate) -> CheckBox&`

Enables tri-state mode.

##### `onStateChanged(std::function<void(int)> handler) -> CheckBox&`

Sets state change handler.

##### `onToggled(std::function<void(bool)> handler) -> CheckBox&`

Sets toggle handler.

#### Usage Example

```cpp
auto checkBox = std::make_unique<Components::CheckBox>();
checkBox->text("Enable notifications")
        .checked(true)
        .onToggled([](bool checked) {
            qDebug() << "Checkbox toggled:" << checked;
        });
checkBox->initialize();
```

### ComboBox Class

Dropdown selection component.

#### Methods

##### `items(const QStringList& items) -> ComboBox&`

Sets all items at once.

##### `addItem(const QString& item) -> ComboBox&`

Adds a single item.

##### `addItems(const QStringList& items) -> ComboBox&`

Adds multiple items.

##### `currentIndex(int index) -> ComboBox&`

Sets the current selection index.

##### `currentText(const QString& text) -> ComboBox&`

Sets the current selection by text.

##### `editable(bool editable) -> ComboBox&`

Makes the combo box editable.

##### `onCurrentIndexChanged(std::function<void(int)> handler) -> ComboBox&`

Sets index change handler.

##### `onCurrentTextChanged(std::function<void(const QString&)> handler) -> ComboBox&`

Sets text change handler.

#### Usage Example

```cpp
auto comboBox = std::make_unique<Components::ComboBox>();
comboBox->addItem("Option 1")
        .addItem("Option 2")
        .addItem("Option 3")
        .currentIndex(0)
        .onCurrentTextChanged([](const QString& text) {
            qDebug() << "Selected:" << text;
        });
comboBox->initialize();
```

### SpinBox Class

Numeric input with spin controls.

#### Methods

##### `value(int value) -> SpinBox&`

Sets the current value.

##### `minimum(int min) -> SpinBox&`

Sets minimum value.

##### `maximum(int max) -> SpinBox&`

Sets maximum value.

##### `step(int step) -> SpinBox&`

Sets step size.

##### `suffix(const QString& suffix) -> SpinBox&`

Sets value suffix.

##### `prefix(const QString& prefix) -> SpinBox&`

Sets value prefix.

##### `onValueChanged(std::function<void(int)> handler) -> SpinBox&`

Sets value change handler.

#### Usage Example

```cpp
auto spinBox = std::make_unique<Components::SpinBox>();
spinBox->minimum(0)
       .maximum(100)
       .value(50)
       .suffix("%")
       .onValueChanged([](int value) {
           qDebug() << "Value changed:" << value;
       });
spinBox->initialize();
```

### Slider Class

Value selection slider component.

#### Methods

##### `value(int value) -> Slider&`

Sets the current value.

##### `minimum(int min) -> Slider&`

Sets minimum value.

##### `maximum(int max) -> Slider&`

Sets maximum value.

##### `orientation(Qt::Orientation orientation) -> Slider&`

Sets slider orientation.

##### `tickPosition(QSlider::TickPosition position) -> Slider&`

Sets tick mark position.

##### `tickInterval(int interval) -> Slider&`

Sets tick mark interval.

##### `onValueChanged(std::function<void(int)> handler) -> Slider&`

Sets value change handler.

#### Usage Example

```cpp
auto slider = std::make_unique<Components::Slider>();
slider->minimum(0)
      .maximum(100)
      .value(50)
      .orientation(Qt::Horizontal)
      .tickPosition(QSlider::TicksBelow)
      .onValueChanged([](int value) {
          qDebug() << "Slider value:" << value;
      });
slider->initialize();
```

## Display Components

### Label Class

Text and image display component.

#### Methods

##### `text(const QString& text) -> Label&`

Sets the label text.

##### `pixmap(const QPixmap& pixmap) -> Label&`

Sets the label image.

##### `alignment(Qt::Alignment alignment) -> Label&`

Sets text alignment.

##### `wordWrap(bool wrap) -> Label&`

Enables word wrapping.

##### `openExternalLinks(bool open) -> Label&`

Enables external link opening.

#### Usage Example

```cpp
auto label = std::make_unique<Components::Label>();
label->text("Welcome to DeclarativeUI")
     .alignment(Qt::AlignCenter)
     .wordWrap(true)
     .style("QLabel { font-size: 18px; font-weight: bold; }");
label->initialize();
```

### ProgressBar Class

Progress indication component.

#### Methods

##### `value(int value) -> ProgressBar&`

Sets the current progress value.

##### `minimum(int min) -> ProgressBar&`

Sets minimum value.

##### `maximum(int max) -> ProgressBar&`

Sets maximum value.

##### `format(const QString& format) -> ProgressBar&`

Sets progress text format.

##### `textVisible(bool visible) -> ProgressBar&`

Shows or hides progress text.

##### `orientation(Qt::Orientation orientation) -> ProgressBar&`

Sets progress bar orientation.

#### Usage Example

```cpp
auto progressBar = std::make_unique<Components::ProgressBar>();
progressBar->minimum(0)
           .maximum(100)
           .value(75)
           .format("%p% Complete")
           .textVisible(true);
progressBar->initialize();
```

### TextEdit Class

Multi-line text display and editing component.

#### Methods

##### `text(const QString& text) -> TextEdit&`

Sets the text content.

##### `html(const QString& html) -> TextEdit&`

Sets HTML content.

##### `readOnly(bool readonly) -> TextEdit&`

Sets read-only mode.

##### `acceptRichText(bool accept) -> TextEdit&`

Enables rich text support.

##### `lineWrapMode(QTextEdit::LineWrapMode mode) -> TextEdit&`

Sets line wrap mode.

##### `onTextChanged(std::function<void()> handler) -> TextEdit&`

Sets text change handler.

#### Usage Example

```cpp
auto textEdit = std::make_unique<Components::TextEdit>();
textEdit->text("Enter your comments here...")
        .acceptRichText(true)
        .lineWrapMode(QTextEdit::WidgetWidth)
        .onTextChanged([]() {
            qDebug() << "Text content changed";
        });
textEdit->initialize();
```

## Container Components

### Container Class

Generic container widget for grouping other components.

#### Methods

##### `layout(QLayout* layout) -> Container&`

Sets the container layout.

##### `addWidget(QWidget* widget) -> Container&`

Adds a widget to the container.

##### `spacing(int spacing) -> Container&`

Sets layout spacing.

##### `margins(int left, int top, int right, int bottom) -> Container&`

Sets layout margins.

### GroupBox Class

Grouped controls with border and title.

#### Methods

##### `title(const QString& title) -> GroupBox&`

Sets the group box title.

##### `checkable(bool checkable) -> GroupBox&`

Makes the group box checkable.

##### `checked(bool checked) -> GroupBox&`

Sets the checked state.

##### `flat(bool flat) -> GroupBox&`

Sets flat appearance.

#### Usage Example

```cpp
auto groupBox = std::make_unique<Components::GroupBox>();
groupBox->title("Settings")
        .checkable(true)
        .checked(true);
groupBox->initialize();
```

### TabWidget Class

Tabbed interface component.

#### Methods

##### `addTab(QWidget* widget, const QString& label) -> TabWidget&`

Adds a new tab.

##### `currentIndex(int index) -> TabWidget&`

Sets the current tab.

##### `tabPosition(QTabWidget::TabPosition position) -> TabWidget&`

Sets tab position.

##### `tabsClosable(bool closable) -> TabWidget&`

Enables tab closing.

##### `onCurrentChanged(std::function<void(int)> handler) -> TabWidget&`

Sets tab change handler.

## Advanced Components

### TableView Class

Tabular data display component.

#### Methods

##### `model(QAbstractItemModel* model) -> TableView&`

Sets the data model.

##### `selectionBehavior(QAbstractItemView::SelectionBehavior behavior) -> TableView&`

Sets selection behavior.

##### `selectionMode(QAbstractItemView::SelectionMode mode) -> TableView&`

Sets selection mode.

##### `sortingEnabled(bool enabled) -> TableView&`

Enables column sorting.

##### `alternatingRowColors(bool alternate) -> TableView&`

Enables alternating row colors.

### TreeView Class

Hierarchical data display component.

#### Methods

##### `model(QAbstractItemModel* model) -> TreeView&`

Sets the tree model.

##### `rootIsDecorated(bool decorated) -> TreeView&`

Shows root decoration.

##### `uniformRowHeights(bool uniform) -> TreeView&`

Sets uniform row heights.

##### `expandsOnDoubleClick(bool expand) -> TreeView&`

Enables double-click expansion.

## Usage Patterns

### Component Creation with DeclarativeBuilder

```cpp
using namespace DeclarativeUI;

auto widget = Core::create<QWidget>()
    .layout<QVBoxLayout>()
    .child<Components::Button>([](auto& btn) {
        btn.text("Save")
           .onClick([]() { /* save logic */ });
    })
    .child<Components::LineEdit>([](auto& edit) {
        edit.placeholder("Enter filename...")
            .onReturnPressed([]() { /* save logic */ });
    })
    .build();
```

### State Binding Integration

```cpp
auto& state = StateManager::instance();
state.setState("counter", 0);

auto label = std::make_unique<Components::Label>();
label->bindProperty("text", []() {
    auto counter = StateManager::instance().getState<int>("counter");
    return QString("Count: %1").arg(counter->get());
});
label->initialize();
```

### JSON Configuration

Components can be configured via JSON:

```json
{
  "type": "Button",
  "properties": {
    "text": "Click Me",
    "enabled": true,
    "styleSheet": "QPushButton { background-color: blue; }"
  },
  "events": {
    "clicked": "handleButtonClick"
  }
}
```

## Best Practices

### Component Initialization

- Always call `initialize()` after configuration
- Set up event handlers before initialization
- Use fluent interface for clean configuration

### Memory Management

- Components use RAII for automatic cleanup
- Parent-child relationships are managed automatically
- Use smart pointers for component ownership

### Performance

- Initialize components only when needed
- Use property binding for dynamic updates
- Batch property changes when possible

### Styling

- Use consistent styling across components
- Leverage Qt's stylesheet system
- Consider theme support for applications
