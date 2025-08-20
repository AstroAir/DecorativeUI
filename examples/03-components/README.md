# Component Examples

Comprehensive showcase of UI components and patterns available in DeclarativeUI.

## Prerequisites

- Completed **01-getting-started** and **02-fundamentals** examples
- Understanding of state management and property binding
- Familiarity with Qt widget concepts

## Examples in this Category

### Basic Components (`basic-components/`)

**Difficulty:** ⭐⭐☆☆☆
**Concepts:** Core UI components, basic styling, component properties

Comprehensive demonstration of fundamental UI components.

**Components covered:**

- Button - Interactive buttons with various styles
- Label - Text display with formatting options
- LineEdit - Text input fields
- Layout containers - Basic layout management

**What you'll learn:**

- Using core UI components
- Setting component properties
- Basic styling and appearance
- Component interaction patterns

**Run this example:**

```bash
cmake --build . --target BasicComponentsExample
./examples/03-components/BasicComponentsExample
```

### Input Components (`input-components/`)

**Difficulty:** ⭐⭐⭐☆☆
**Concepts:** User input, form controls, validation, data collection

Showcase of components designed for user input and data collection.

**Components covered:**

- CheckBox - Boolean input controls
- ComboBox - Dropdown selection lists
- SpinBox - Numeric input with increment/decrement
- Slider - Range-based input controls
- RadioButton - Exclusive selection groups

**What you'll learn:**

- Handling different types of user input
- Form validation patterns
- Input component customization
- Data collection and processing

**Run this example:**

```bash
cmake --build . --target InputComponentsExample
./examples/03-components/InputComponentsExample
```

### Layout Components (`layout-components/`)

**Difficulty:** ⭐⭐⭐☆☆
**Concepts:** Layout management, responsive design, container components

Demonstration of layout managers and container components for organizing UI elements.

**Layouts covered:**

- VBoxLayout - Vertical arrangement
- HBoxLayout - Horizontal arrangement
- GridLayout - Grid-based positioning
- FormLayout - Form-style organization
- Nested layouts - Complex layout combinations

**What you'll learn:**

- Different layout strategies
- Responsive design principles
- Layout nesting and composition
- Space management and alignment

**Run this example:**

```bash
cmake --build . --target LayoutComponentsExample
./examples/03-components/LayoutComponentsExample
```

### Enhanced Components (`custom-components/enhanced-components.cpp`)

**Difficulty:** ⭐⭐⭐⭐☆
**Concepts:** Advanced component features, customization, enhanced functionality

Advanced usage of components with enhanced features and customization.

**What you'll learn:**

- Advanced component customization
- Enhanced styling and theming
- Component composition patterns
- Performance optimization for components

**Run this example:**

```bash
cmake --build . --target EnhancedComponentsExample
./examples/03-components/EnhancedComponentsExample
```

### New Components (`custom-components/new-components.cpp`)

**Difficulty:** ⭐⭐⭐⭐☆
**Concepts:** Latest component additions, cutting-edge features

Showcase of the newest components and features added to DeclarativeUI.

**What you'll learn:**

- Latest component additions
- Cutting-edge UI patterns
- Modern component features
- Future-oriented design patterns

**Run this example:**

```bash
cmake --build . --target NewComponentsExample
./examples/03-components/NewComponentsExample
```

## Learning Path

1. **Foundation:** `basic-components/` - Master the core components
2. **Input handling:** `input-components/` - Learn data collection
3. **Layout mastery:** `layout-components/` - Organize your UI
4. **Advanced usage:** `enhanced-components/` - Customize and optimize
5. **Stay current:** `new-components/` - Explore latest features

## Component Categories

### Display Components

- **Label** - Text and rich content display
- **Image** - Image and icon display
- **ProgressBar** - Progress indication
- **StatusBar** - Application status

### Input Components

- **LineEdit** - Single-line text input
- **TextEdit** - Multi-line text input
- **SpinBox** - Numeric input
- **Slider** - Range selection
- **CheckBox** - Boolean selection
- **RadioButton** - Exclusive selection
- **ComboBox** - Dropdown selection

### Container Components

- **GroupBox** - Logical grouping
- **TabWidget** - Tabbed interface
- **ScrollArea** - Scrollable content
- **Splitter** - Resizable panes

### Action Components

- **Button** - User actions
- **ToolButton** - Toolbar actions
- **MenuBar** - Application menus
- **ToolBar** - Action toolbars

## Best Practices

### Component Selection

1. **Choose the right component** - Match component to use case
2. **Consider accessibility** - Ensure components are accessible
3. **Think responsive** - Components should work on different screen sizes
4. **Plan for localization** - Consider text expansion and RTL languages

### Styling and Theming

1. **Use consistent styling** - Maintain visual coherence
2. **Leverage themes** - Use theme systems for maintainability
3. **Consider dark mode** - Support multiple color schemes
4. **Test with real content** - Use realistic data for testing

### Performance

1. **Lazy loading** - Load components when needed
2. **Efficient updates** - Minimize unnecessary redraws
3. **Memory management** - Properly manage component lifecycle
4. **Batch operations** - Group related updates together

## Common Patterns

### Component Composition

```cpp
// Good: Compose complex components from simple ones
auto formGroup = createFormGroup({
    createLabeledInput("Name:", nameEdit),
    createLabeledInput("Email:", emailEdit),
    createButtonRow({"Save", "Cancel"})
});
```

### Property Binding

```cpp
// Good: Bind component properties
bindProperty(slider, "value", progressBar, "value");
bindProperty(checkBox, "checked", button, "enabled");
```

### Event Handling

```cpp
// Good: Use declarative event handling
onClicked(button, [this]() { handleButtonClick(); });
onTextChanged(lineEdit, [this](const QString& text) {
    validateInput(text);
});
```

## Next Steps

After mastering components, explore:

- **04-advanced-features/** - Hot reload and reactive state
- **05-command-system/** - Command-based architecture
- **06-applications/** - Complete applications using components
