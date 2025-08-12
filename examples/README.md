# DeclarativeUI Examples

This directory contains all examples for the DeclarativeUI project, organized by learning progression and feature focus.

## 🎯 Quick Start

**New to DeclarativeUI?** Start here:
1. **01-getting-started/hello-world/** - Your first DeclarativeUI app
2. **01-getting-started/basic-ui/** - Learn basic components
3. **02-fundamentals/state-management/** - Understand reactive state

## 📁 Directory Structure

```text
examples/
├── README.md                           # This file
├── CMakeLists.txt                      # Build configuration
├── 01-getting-started/                 # 🌱 Absolute beginner examples
│   ├── hello-world/                    # Minimal DeclarativeUI app
│   ├── basic-ui/                       # Basic UI components
│   └── simple-events/                  # Event handling basics
├── 02-fundamentals/                    # 🏗️ Core concepts
│   ├── state-management/               # Reactive state patterns
│   ├── json-ui-loading/                # JSON-driven UI creation
│   ├── event-handling/                 # Advanced event patterns
│   └── property-binding/               # Reactive property binding
├── 03-components/                      # 🧩 UI component showcase
│   ├── basic-components/               # Core UI components
│   ├── input-components/               # Form and input controls
│   ├── layout-components/              # Layout management
│   └── custom-components/              # Advanced component usage
├── 04-advanced-features/               # 🚀 Advanced DeclarativeUI features
│   ├── hot-reload/                     # Development hot reload
│   ├── reactive-state/                 # Advanced reactive patterns
│   ├── file-watching/                  # File system monitoring
│   └── performance/                    # Performance optimization
├── 05-command-system/                  # ⚡ Command architecture (experimental)
│   ├── command-basics/                 # Command pattern fundamentals
│   ├── command-builder/                # Builder patterns (disabled)
│   ├── json-commands/                  # JSON-driven commands (disabled)
│   └── state-integration/              # Command-state integration (disabled)
├── 06-applications/                    # 📱 Complete sample applications
│   ├── counter-app/                    # Simple counter application
│   ├── todo-app/                       # Full-featured todo app
│   ├── form-example/                   # Form handling and validation
│   ├── settings-app/                   # Configuration management
│   └── dashboard/                      # Complex dashboard application
├── shared/                             # 📦 Shared resources
│   ├── json/                           # JSON UI definitions
│   ├── assets/                         # Images and other assets
│   └── common/                         # Shared utilities
└── templates/                          # 📋 Example templates
    ├── basic-example-template/         # Template for new basic examples
    └── application-template/           # Template for new applications
```

## 🚀 Running Examples

### Build All Examples

```bash
cd build
cmake .. -DBUILD_EXAMPLES=ON
cmake --build . --config Release
```

### Run Specific Examples

```bash
# Getting Started
./examples/01-getting-started/HelloWorldExample
./examples/01-getting-started/BasicUIExample

# Fundamentals
./examples/02-fundamentals/StateManagementExample
./examples/02-fundamentals/JSONUILoadingExample

# Components
./examples/03-components/BasicComponentsExample
./examples/03-components/InputComponentsExample

# Advanced Features
./examples/04-advanced-features/ReactiveStateExample
./examples/04-advanced-features/PerformanceExample

# Command System (experimental)
./examples/05-command-system/CommandBasicsExample

# Applications
./examples/06-applications/TodoAppExample
./examples/06-applications/DashboardExample
```

## 📚 Learning Path

### 🌱 Beginner (Start Here)
**Goal:** Get familiar with DeclarativeUI basics

1. **01-getting-started/hello-world/** - Create your first app
2. **01-getting-started/basic-ui/** - Learn basic components
3. **01-getting-started/simple-events/** - Handle user interactions
4. **02-fundamentals/state-management/** - Understand reactive state

**Time:** 2-4 hours

### 🏗️ Intermediate
**Goal:** Master core concepts and components

1. **02-fundamentals/property-binding/** - Reactive connections
2. **02-fundamentals/json-ui-loading/** - Dynamic UI creation
3. **03-components/basic-components/** - Core UI components
4. **03-components/input-components/** - Form controls
5. **03-components/layout-components/** - Layout management
6. **06-applications/counter-app/** - Simple application

**Time:** 4-8 hours

### 🚀 Advanced
**Goal:** Explore advanced features and patterns

1. **04-advanced-features/reactive-state/** - Advanced reactive patterns
2. **04-advanced-features/hot-reload/** - Development workflow
3. **04-advanced-features/performance/** - Optimization techniques
4. **05-command-system/command-basics/** - Command patterns
5. **06-applications/todo-app/** - Complex application
6. **06-applications/dashboard/** - Advanced UI composition

**Time:** 8-16 hours

### 🎯 Expert
**Goal:** Build production-ready applications

1. **06-applications/form-example/** - Form handling
2. **06-applications/settings-app/** - Configuration management
3. **Create your own application** - Apply all concepts
4. **Contribute to DeclarativeUI** - Help improve the framework

**Time:** 16+ hours

## 🎨 Example Categories

### 🌱 01-getting-started
Perfect for absolute beginners. These examples use minimal code and focus on single concepts.

- **Difficulty:** ⭐☆☆☆☆
- **Prerequisites:** Basic C++ knowledge
- **Focus:** Getting familiar with DeclarativeUI

### 🏗️ 02-fundamentals
Core concepts that every DeclarativeUI developer should understand.

- **Difficulty:** ⭐⭐⭐☆☆
- **Prerequisites:** Completed getting-started
- **Focus:** State management, property binding, JSON UI

### 🧩 03-components
Comprehensive showcase of UI components and layout patterns.

- **Difficulty:** ⭐⭐⭐☆☆
- **Prerequisites:** Understanding of fundamentals
- **Focus:** UI components, layouts, styling

### 🚀 04-advanced-features
Advanced DeclarativeUI features for sophisticated applications.

- **Difficulty:** ⭐⭐⭐⭐☆
- **Prerequisites:** Component mastery
- **Focus:** Hot reload, reactive state, performance

### ⚡ 05-command-system (Experimental)
Command-based architecture patterns. Some examples currently disabled.

- **Difficulty:** ⭐⭐⭐⭐⭐
- **Prerequisites:** Advanced features understanding
- **Focus:** Command patterns, MVVM architecture

### 📱 06-applications
Complete sample applications demonstrating real-world usage.

- **Difficulty:** ⭐⭐⭐⭐☆
- **Prerequisites:** All previous categories
- **Focus:** Application architecture, best practices

## 🛠️ Key Concepts Demonstrated

### State Management
- **Property binding** - Reactive connections between components
- **Reactive updates** - Automatic UI synchronization
- **State synchronization** - Keeping data consistent
- **Event-driven updates** - Responding to user actions

### UI Construction
- **Declarative UI building** - Describe what you want, not how
- **Component composition** - Building complex UIs from simple parts
- **Layout management** - Organizing UI elements effectively
- **Dynamic UI creation** - Creating UI at runtime

### Command System
- **Command-based architecture** - Encapsulating operations as objects
- **Builder patterns** - Fluent interfaces for complex operations
- **Integration patterns** - Connecting different system parts
- **JSON-driven UI** - Creating UI from configuration

### Hot Reload
- **File watching** - Monitoring file system changes
- **Dynamic reloading** - Updating running applications
- **Performance monitoring** - Measuring and optimizing performance
- **Development workflow** - Improving developer experience

## 📋 Adding New Examples

### Basic Example
1. Choose appropriate category (01-06)
2. Create subdirectory with descriptive kebab-case name
3. Add source files and README.md
4. Update category CMakeLists.txt
5. Test and document

### Application Example
1. Create subdirectory in `06-applications/`
2. Include comprehensive README.md
3. Demonstrate multiple DeclarativeUI features
4. Follow application architecture best practices
5. Include error handling and user feedback

## 📦 Dependencies

Examples depend on:
- **DeclarativeUI library** - Core framework
- **Components library** - UI component implementations
- **Qt6::Core** - Qt core functionality
- **Qt6::Widgets** - Widget-based UI
- **Qt6::Network** - Network functionality (some examples)

## 📁 Resources

Examples use resources from:
- **shared/json/** - JSON UI definitions
- **shared/assets/** - Images and other assets
- **shared/common/** - Shared utilities and helpers

Resources are automatically copied to build directory during build.

## 🔧 Troubleshooting

### Build Issues
- **Qt6 not found:** Ensure Qt6 is installed and in PATH
- **DeclarativeUI not found:** Build DeclarativeUI library first
- **CMake errors:** Check CMake version (3.16+ required)

### Runtime Issues
- **Missing libraries:** Check library paths and dependencies
- **Resource not found:** Verify resources are copied to build directory
- **Crashes:** Check console output for error messages

### Performance Issues
- **Slow startup:** Check for expensive initialization
- **UI lag:** Profile using performance example
- **Memory leaks:** Use memory profiling tools

## 🤝 Contributing

### Adding Examples
1. Follow existing naming conventions
2. Include comprehensive documentation
3. Test on multiple platforms
4. Follow code style guidelines

### Improving Documentation
1. Keep README files up to date
2. Add clear learning objectives
3. Include troubleshooting information
4. Provide context and background

### Reporting Issues
1. Use GitHub issues for bug reports
2. Include minimal reproduction case
3. Specify platform and Qt version
4. Provide clear steps to reproduce

## 📖 Additional Resources

- **DeclarativeUI Documentation** - Complete framework documentation
- **Qt Documentation** - Underlying Qt concepts and APIs
- **Design Patterns** - Software design pattern references
- **C++ Best Practices** - Modern C++ programming guidelines
