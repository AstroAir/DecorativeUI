# Enhanced Components System

The Enhanced Components System provides a comprehensive set of enhancements for existing DeclarativeUI components, adding accessibility, validation, visual effects, and advanced functionality.

## Overview

As of the latest version, enhanced functionality has been integrated directly into the base components (Button, LineEdit, etc.). The ComponentEnhancer system remains available for composition-based enhancement of any Qt widget.

The enhancement system uses composition and decoration patterns to layer additional functionality on top of existing components. This approach allows you to:

- Add accessibility features to any component
- Implement validation and error handling
- Apply visual effects and styling
- Add keyboard shortcuts and navigation
- Implement auto-completion and suggestions
- Create responsive and interactive behaviors

## Enhanced Base Components

The base components now include all enhanced functionality:

### Enhanced Button Features

```cpp
#include "Components/Button.hpp"
using namespace DeclarativeUI::Components;

auto button = std::make_unique<Button>();
button->text("Save Document")
      ->tooltip("Save the current document")
      ->accessibleName("Save Button")
      ->shortcut(QKeySequence::Save)
      ->dropShadow(true)
      ->hoverEffect(true)
      ->borderRadius(8)
      ->onClick([]() { /* save logic */ });
button->initialize();
```

### Enhanced LineEdit Features

```cpp
#include "Components/LineEdit.hpp"
using namespace DeclarativeUI::Components;

auto email_field = std::make_unique<LineEdit>();
email_field->placeholder("Enter your email")
           ->tooltip("Email address for notifications")
           ->accessibleName("Email Address")
           ->required(true)
           ->email(true)
           ->autoComplete({"user@example.com", "admin@company.com"})
           ->selectAllOnFocus(true)
           ->borderRadius(4);
email_field->initialize();
```

## Key Features

### 🔧 Non-Intrusive Enhancement

- Works with existing components without modification
- Uses composition pattern for clean separation
- Maintains backward compatibility
- Easy to apply and remove

### ♿ Comprehensive Accessibility

- ARIA roles and properties
- Screen reader support
- Keyboard navigation
- Focus management
- Tooltip integration

### ✅ Advanced Validation

- Real-time validation
- Custom validation functions
- Visual feedback
- Error messaging
- Required field support

### 🎨 Visual Effects

- Drop shadows
- Hover effects
- Focus indicators
- Border styling
- Animations

### ⌨️ Keyboard Support

- Custom shortcuts
- Tab navigation
- Focus management
- Escape handling

## Quick Start

### Basic Enhancement

```cpp
#include "Components/Enhanced/ComponentEnhancer.hpp"
using namespace DeclarativeUI::Components::Enhanced;

// Create a regular Qt widget
auto button = new QPushButton("Click Me");

// Enhance it with accessibility and visual effects
ComponentEnhancer::enhance(button,
    enhance()
        .accessibleName("Action Button")
        .tooltip("Click to perform action")
        .dropShadow(true)
        .hoverEffect(true)
        .shortcut(QKeySequence("Ctrl+A"))
        .build()
);
```

### Validation Enhancement

```cpp
auto email_input = new QLineEdit();

ComponentEnhancer::enhance(email_input,
    enhance()
        .accessibleName("Email Address")
        .tooltip("Enter your email address")
        .required(true)
        .validator([](QWidget* widget) -> bool {
            auto* edit = qobject_cast<QLineEdit*>(widget);
            QString text = edit->text();
            return text.contains("@") && text.contains(".");
        })
        .validationError("Please enter a valid email address")
        .validateOnChange(true)
        .focusEffect(true)
        .build()
);
```

### Auto-Completion Enhancement

```cpp
auto country_input = new QLineEdit();
QStringList countries = {"United States", "Canada", "United Kingdom", "Australia"};

ComponentEnhancer::enhance(country_input,
    enhance()
        .accessibleName("Country Selection")
        .tooltip("Start typing to see suggestions")
        .autoComplete(countries)
        .selectAllOnFocus(true)
        .build()
);
```

## Enhancement Types

### Accessibility Enhancements

```cpp
enhance()
    .accessibleName("Button Name")           // Screen reader name
    .accessibleDescription("Detailed desc")  // Screen reader description
    .tooltip("Tooltip text")                 // Visual tooltip
    .role(AccessibilityRole::Button)         // ARIA role
    .tabIndex(1)                            // Tab order
```

### Visual Enhancements

```cpp
enhance()
    .dropShadow(true, QColor(0, 0, 0, 80))  // Drop shadow effect
    .hoverEffect(true)                       // Hover state styling
    .focusEffect(true)                       // Focus indicator
    .borderRadius(8)                         // Rounded corners
    .borderColor(QColor("#3498db"))          // Border color
    .focusColor(QColor("#e74c3c"))          // Focus border color
```

### Validation Enhancements

```cpp
enhance()
    .required(true)                          // Mark as required
    .validator([](QWidget* w) { return true; }) // Custom validation
    .validationError("Error message")        // Error text
    .validateOnChange(true)                  // Validate while typing
    .validateOnFocusLost(true)              // Validate on blur
```

### Keyboard Enhancements

```cpp
enhance()
    .shortcut(QKeySequence("Ctrl+S"))       // Keyboard shortcut
    .selectAllOnFocus(true)                 // Select all on focus
    .clearOnEscape(true)                    // Clear on Escape key
```

### Input Enhancements

```cpp
enhance()
    .autoComplete({"Option 1", "Option 2"}) // Auto-completion
    .inputMask("000-000-0000")              // Input formatting
    .customValidator(new QRegExpValidator()) // Custom validator
    .debounceDelay(300)                     // Debounce input events
```

## Component-Specific Enhancements

### Enhanced Buttons

```cpp
// Button with comprehensive enhancements
auto save_button = new QPushButton("Save");

ComponentEnhancer::enhanceButton(save_button,
    enhance()
        .accessibleName("Save Document")
        .tooltip("Save the current document (Ctrl+S)")
        .shortcut(QKeySequence::Save)
        .dropShadow(true)
        .hoverEffect(true)
        .validator([](QWidget* w) {
            // Only enable if document is modified
            return true; // Your logic here
        })
        .build()
);
```

### Enhanced Line Edits

```cpp
// Email input with validation and auto-completion
auto email_field = new QLineEdit();

ComponentEnhancer::enhanceLineEdit(email_field,
    enhance()
        .accessibleName("Email Address")
        .tooltip("Enter your email address")
        .required(true)
        .validator([](QWidget* widget) -> bool {
            auto* edit = qobject_cast<QLineEdit*>(widget);
            return edit && edit->text().contains("@");
        })
        .validationError("Please enter a valid email")
        .autoComplete({"user@example.com", "admin@company.com"})
        .selectAllOnFocus(true)
        .build()
);
```

### Enhanced Check Boxes

```cpp
// Checkbox with accessibility and validation
auto terms_checkbox = new QCheckBox("I agree to the terms");

ComponentEnhancer::enhanceCheckBox(terms_checkbox,
    enhance()
        .accessibleName("Terms Agreement")
        .accessibleDescription("Check to agree to terms and conditions")
        .tooltip("You must agree to continue")
        .required(true)
        .validator([](QWidget* widget) -> bool {
            auto* checkbox = qobject_cast<QCheckBox*>(widget);
            return checkbox && checkbox->isChecked();
        })
        .validationError("You must agree to the terms")
        .build()
);
```

## Batch Enhancement

### Enhance Container

```cpp
// Enhance all widgets in a container
auto form = new QWidget();
// ... add widgets to form ...

ComponentEnhancer::enhanceContainer(form,
    enhance()
        .hoverEffect(true)
        .focusEffect(true)
        .borderRadius(4)
        .build()
);
```

### Enhance Form Fields

```cpp
// Enhance specific form fields
std::map<QString, EnhancementConfig> field_configs = {
    {"nameEdit", enhance().required(true).accessibleName("Full Name").build()},
    {"emailEdit", enhance().required(true).validator(emailValidator).build()},
    {"phoneEdit", enhance().inputMask("(000) 000-0000").build()}
};

ComponentEnhancer::enhanceForm(form, field_configs);
```

## Convenience Macros

For common enhancement patterns, use the provided macros:

```cpp
// Quick accessibility enhancement
ENHANCE_ACCESSIBILITY(widget, "Widget Name", "Widget Description");

// Quick validation enhancement
ENHANCE_VALIDATION(widget, validatorFunction, "Error message");

// Quick visual enhancement
ENHANCE_VISUAL(widget);

// Quick tooltip
ENHANCE_TOOLTIP(widget, "Tooltip text");
```

## Event Handling

Enhanced components can have custom event handlers:

```cpp
enhance()
    .onValidationChanged([](bool valid) {
        qDebug() << "Validation changed:" << valid;
    })
    .onFocusChanged([](bool focused) {
        qDebug() << "Focus changed:" << focused;
    })
    .onHoverEnter([]() {
        qDebug() << "Mouse entered";
    })
    .onHoverLeave([]() {
        qDebug() << "Mouse left";
    })
```

## Removing Enhancements

```cpp
// Remove all enhancements from a widget
ComponentEnhancer::removeEnhancements(widget);

// Remove specific enhancement types
ComponentEnhancer::removeAccessibilityEnhancements(widget);
ComponentEnhancer::removeVisualEnhancements(widget);
ComponentEnhancer::removeValidationEnhancements(widget);
```

## Best Practices

1. **Apply Early**: Enhance components immediately after creation
2. **Use Meaningful Names**: Provide clear accessible names and descriptions
3. **Validate Appropriately**: Use real-time validation for better UX
4. **Group Related Enhancements**: Use the fluent interface to group related features
5. **Test Accessibility**: Test with screen readers and keyboard navigation
6. **Performance**: Be mindful of enhancement overhead for large forms

## Integration with DeclarativeUI

The enhancement system integrates seamlessly with existing DeclarativeUI features:

- Uses the existing AccessibilityManager
- Leverages the Animation system for effects
- Integrates with the Theme system
- Works with the State management system
- Compatible with the Command system

## Examples

See the comprehensive example in `example/showcase/ComponentGallery.cpp` for detailed usage examples of all enhancement features.

## API Reference

For complete API documentation, see the header files:

- `ComponentEnhancer.hpp` - Composition-based enhancement system for any Qt widget
- `../Button.hpp` - Enhanced button component with integrated advanced features
- `../LineEdit.hpp` - Enhanced line edit component with integrated validation and formatting
