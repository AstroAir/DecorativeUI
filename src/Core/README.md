# Core Directory

This directory contains the fundamental components and utilities that form the foundation of the DeclarativeUI framework.

## Responsibilities

The Core directory provides:

1. **Base UI Components** - Core classes for building user interfaces
2. **Declarative Builder Pattern** - Fluent API for constructing UI elements
3. **Animation Integration** - Modern animation system integration
4. **Memory Management** - Smart pointer utilities and memory optimization
5. **Error Handling** - Comprehensive error handling and validation
6. **Performance Utilities** - Caching, parallel processing, and optimization tools

## Key Components

### UIElement (UIElement.hpp/cpp)
- **Purpose**: Base class for all UI elements in the framework
- **Features**: 
  - Property binding and state management
  - Animation support using the new Animation engine
  - Event handling and lifecycle management
  - Performance monitoring and metrics
- **Animation Migration**: Now uses the centralized Animation engine instead of legacy QPropertyAnimation

### DeclarativeBuilder (DeclarativeBuilder.hpp/cpp)
- **Purpose**: Fluent API for building UI components declaratively
- **Features**:
  - Type-safe widget creation
  - Property setting with validation
  - Layout management
  - Event binding
  - Error handling with detailed diagnostics

### ComponentUtils (ComponentUtils.hpp)
- **Purpose**: Utility functions for component operations
- **Features**:
  - Animation utilities (migrated to new Animation system)
  - Widget manipulation helpers
  - Type-safe operations using concepts
  - Error handling integration

### Animation Integration
- **Migration Completed**: All animation functionality now uses the centralized Animation engine
- **Benefits**:
  - Consistent animation behavior across components
  - Better performance with modern animation algorithms
  - Centralized animation management
  - Support for complex animation sequences

### Memory Management (MemoryManager.hpp/cpp)
- **Purpose**: Optimized memory allocation and management
- **Features**:
  - Custom allocators for UI components
  - Memory pool management
  - Leak detection and monitoring

### Error Handling (ErrorHandling.hpp/cpp)
- **Purpose**: Comprehensive error handling system
- **Features**:
  - Exception types for different error categories
  - Safe execution wrappers
  - Logging and debugging support

## Recent Changes

### Animation System Migration
- **Completed**: Migration from legacy QPropertyAnimation to new Animation engine
- **Updated Components**:
  - UIElement animation methods (animate, fadeIn, fadeOut, slideIn, slideOut)
  - ComponentUtils animation utilities
  - AnimationConfig bridge for backward compatibility
- **Benefits**:
  - Centralized animation logic
  - Improved performance
  - Better animation coordination
  - Modern easing functions and effects

### Build System Updates
- **Added**: Animation library dependency
- **Updated**: CMakeLists.txt to properly link Animation components
- **Verified**: All tests pass with new animation system

## Dependencies

- **Qt6**: Core, Widgets modules
- **Animation**: New centralized animation engine
- **Internal**: Concepts, SmartPointers, ErrorHandling

## Usage Examples

```cpp
// Creating a UI element with animations
auto element = UIElement::create<QPushButton>("Click Me")
    .setProperty("text", "Animated Button")
    .fadeIn(AnimationConfig{500ms, QEasingCurve::OutCubic})
    .onClicked([](){ /* handle click */ });

// Using DeclarativeBuilder
auto widget = DeclarativeBuilder::create<QWidget>()
    .setLayout<QVBoxLayout>()
    .addChild<QPushButton>("Button 1")
    .addChild<QPushButton>("Button 2")
    .build();
```

## Testing

All Core components are thoroughly tested:
- Unit tests in `tests/unit/CoreTest.cpp`
- Integration tests verify animation system migration
- Performance tests ensure optimization goals are met

## Future Enhancements

- Enhanced animation sequences and groups
- Additional utility functions for common UI patterns
- Performance optimizations for large UI hierarchies
- Extended error handling and diagnostics
