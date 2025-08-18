# Advanced Composite Components

The Advanced Composite Components provide sophisticated, high-level UI components built by combining multiple basic components into complex, feature-rich widgets. These components offer enterprise-grade functionality with modern UX patterns.

## Overview

Advanced components are built using composition patterns, combining multiple basic DeclarativeUI components to create complex, reusable UI elements. Each component provides:

- **Rich Functionality** - Complex behaviors built from simple components
- **Modern UX Patterns** - Contemporary interface designs and interactions
- **Accessibility Support** - Full WCAG compliance and screen reader support
- **Customizable Styling** - Comprehensive theming and visual customization
- **Event-Driven Architecture** - Reactive programming patterns
- **Performance Optimized** - Efficient rendering and memory usage

## Available Components

### 🔍 SearchBox
A comprehensive search input component with auto-completion, filtering, and advanced search features.

**Features:**
- Auto-completion with customizable data sources
- Real-time search suggestions
- Search history management
- Advanced filtering options
- Keyboard navigation support
- Customizable result templates

### 📊 DataTable
A powerful data grid component with sorting, filtering, pagination, and editing capabilities.

**Features:**
- Virtual scrolling for large datasets
- Column sorting and filtering
- In-line editing with validation
- Row selection and bulk operations
- Pagination and infinite scrolling
- Export functionality (CSV, JSON, Excel)
- Responsive column resizing
- Custom cell renderers

### 🎛️ PropertyEditor
A dynamic property editing component that automatically generates appropriate editors for different data types.

**Features:**
- Automatic editor generation based on data types
- Support for nested objects and arrays
- Custom editor registration
- Validation and error display
- Undo/redo functionality
- Real-time property binding

### 📋 FormBuilder
A dynamic form generation component that creates forms from JSON schemas or data models.

**Features:**
- JSON Schema-based form generation
- Dynamic field addition/removal
- Complex validation rules
- Conditional field visibility
- Multi-step form support
- Form state management

### 🗂️ FileExplorer
A complete file system browser component with tree view, details view, and file operations.

**Features:**
- Multiple view modes (tree, list, grid, details)
- File operations (copy, move, delete, rename)
- File type filtering and search
- Thumbnail generation for images
- Context menus and keyboard shortcuts
- Drag and drop support

### 📈 ChartWidget
A comprehensive charting component supporting multiple chart types with interactive features.

**Features:**
- Multiple chart types (line, bar, pie, scatter, area)
- Interactive zoom and pan
- Real-time data updates
- Custom styling and themes
- Export to various formats
- Responsive design

### 🎨 ColorPicker
An advanced color selection component with multiple color models and palette management.

**Features:**
- Multiple color models (RGB, HSV, HSL, CMYK)
- Color palette management
- Eyedropper tool
- Color history
- Accessibility-friendly contrast checking
- Custom color spaces

### 📅 DateTimePicker
A sophisticated date and time selection component with calendar views and time zones.

**Features:**
- Multiple calendar views (month, year, decade)
- Time zone support
- Date range selection
- Custom date formats
- Localization support
- Keyboard navigation

## Architecture

### Composition Pattern

Advanced components use composition rather than inheritance, combining multiple basic components:

```cpp
class SearchBox : public Core::UIElement {
private:
    std::unique_ptr<LineEdit> search_input_;
    std::unique_ptr<Container> suggestions_container_;
    std::unique_ptr<ListView> suggestions_list_;
    std::unique_ptr<Button> clear_button_;
    std::unique_ptr<Button> search_button_;
};
```

### Event-Driven Design

Components communicate through a robust event system:

```cpp
// SearchBox emits events for different user actions
searchBox->onSearchChanged([](const QString& query) {
    // Handle search query changes
});

searchBox->onSuggestionSelected([](const SearchSuggestion& suggestion) {
    // Handle suggestion selection
});
```

### State Management

Advanced components maintain complex internal state:

```cpp
class DataTableState {
public:
    QAbstractItemModel* model;
    QList<int> selected_rows;
    QString filter_text;
    QMap<int, Qt::SortOrder> column_sort_order;
    int current_page;
    int items_per_page;
};
```

## Usage Examples

### SearchBox with Auto-completion

```cpp
#include "Components/Advanced/SearchBox.hpp"
using namespace DeclarativeUI::Components::Advanced;

auto searchBox = std::make_unique<SearchBox>();
searchBox->placeholder("Search products...")
         ->autoComplete(true)
         ->suggestions({"Apple iPhone", "Samsung Galaxy", "Google Pixel"})
         ->onSearchChanged([](const QString& query) {
             qDebug() << "Searching for:" << query;
         })
         ->onSuggestionSelected([](const QString& suggestion) {
             qDebug() << "Selected:" << suggestion;
         });
```

### DataTable with Custom Model

```cpp
#include "Components/Advanced/DataTable.hpp"
using namespace DeclarativeUI::Components::Advanced;

auto dataTable = std::make_unique<DataTable>();
dataTable->model(customModel)
         ->sortable(true)
         ->filterable(true)
         ->selectable(true)
         ->pagination(true)
         ->itemsPerPage(50)
         ->onRowSelected([](int row) {
             qDebug() << "Row selected:" << row;
         })
         ->onCellEdited([](int row, int column, const QVariant& value) {
             qDebug() << "Cell edited:" << row << column << value;
         });
```

### PropertyEditor for Settings

```cpp
#include "Components/Advanced/PropertyEditor.hpp"
using namespace DeclarativeUI::Components::Advanced;

auto propertyEditor = std::make_unique<PropertyEditor>();
propertyEditor->object(settingsObject)
             ->groupByCategory(true)
             ->showDescriptions(true)
             ->onPropertyChanged([](const QString& property, const QVariant& value) {
                 qDebug() << "Property changed:" << property << "=" << value;
             });
```

## Integration

### With Enhanced Components

Advanced components work seamlessly with the Enhanced component system:

```cpp
// Enhance a SearchBox with additional accessibility features
ComponentEnhancer::enhance(searchBox.get(),
    enhance()
        .accessibleName("Product Search")
        .tooltip("Search for products in the catalog")
        .shortcut(QKeySequence("Ctrl+F"))
        .build()
);
```

### With Command System

Advanced components integrate with the Command system for complex operations:

```cpp
// DataTable operations as commands
dataTable->onBulkOperation([](const QString& operation, const QList<int>& rows) {
    auto command = CommandBuilder("BulkDataOperation")
        .parameter("operation", operation)
        .parameter("rows", QVariant::fromValue(rows))
        .build();
    
    CommandInvoker::instance().execute(command);
});
```

### With State Management

Advanced components support reactive state binding:

```cpp
// Bind SearchBox to application state
searchBox->bindToState("app.search.query", "text")
         ->bindToState("app.search.suggestions", "suggestions");
```

## Performance Considerations

### Virtual Scrolling

Large data components use virtual scrolling for optimal performance:

```cpp
// DataTable automatically enables virtual scrolling for large datasets
dataTable->virtualScrolling(true)
         ->bufferSize(100)  // Number of items to render outside viewport
         ->itemHeight(32);  // Fixed item height for optimal performance
```

### Lazy Loading

Components support lazy loading of data and resources:

```cpp
// FileExplorer loads directory contents on demand
fileExplorer->lazyLoading(true)
           ->onDirectoryExpanded([](const QString& path) {
               // Load directory contents asynchronously
               return loadDirectoryAsync(path);
           });
```

### Memory Management

Advanced components use smart pointers and RAII for automatic memory management:

```cpp
class SearchBox : public Core::UIElement {
private:
    // All child components are automatically cleaned up
    std::unique_ptr<LineEdit> search_input_;
    std::unique_ptr<Container> suggestions_container_;
    // ... other components
};
```

## Customization

### Styling

Advanced components support comprehensive styling:

```cpp
searchBox->style(R"(
    SearchBox {
        background-color: #ffffff;
        border: 1px solid #cccccc;
        border-radius: 4px;
    }
    
    SearchBox::suggestions {
        background-color: #f9f9f9;
        border: 1px solid #dddddd;
    }
    
    SearchBox::suggestion:hover {
        background-color: #e6f3ff;
    }
)");
```

### Templates

Components support custom templates for rendering:

```cpp
dataTable->cellTemplate("status", [](const QVariant& value) -> QWidget* {
    auto status = value.toString();
    auto label = new QLabel(status);
    
    if (status == "Active") {
        label->setStyleSheet("color: green; font-weight: bold;");
    } else if (status == "Inactive") {
        label->setStyleSheet("color: red;");
    }
    
    return label;
});
```

## Best Practices

1. **Use Composition** - Build complex components by combining simpler ones
2. **Event-Driven Design** - Use events for component communication
3. **State Management** - Maintain clear separation between UI state and business logic
4. **Performance** - Use virtual scrolling and lazy loading for large datasets
5. **Accessibility** - Ensure all components are keyboard navigable and screen reader friendly
6. **Testing** - Write comprehensive tests for complex component behaviors
7. **Documentation** - Document component APIs and usage patterns clearly

## Examples

See the comprehensive examples in `examples/04-advanced-components/` for detailed usage demonstrations of all advanced components.

## API Reference

For complete API documentation, see the individual header files in this directory and the generated API documentation.
