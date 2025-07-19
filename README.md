# DeclarativeUI Build System

This project uses CMake as its build system with a modular structure for easy maintenance and development.

## Project Structure

```text
DeclarativeUI/
├── CMakeLists.txt              # Main build configuration
├── build.bat                   # Windows build script
├── src/
│   ├── Core/                   # Core UI framework
│   ├── Components/             # UI Components library
│   │   └── CMakeLists.txt
│   ├── JSON/                   # JSON parsing and validation
│   ├── HotReload/              # Hot reload functionality
│   ├── Binding/                # Property binding system
│   ├── Command/                # Command system
│   ├── Examples/               # Example applications
│   │   └── CMakeLists.txt
│   └── Tests/                  # Unit tests
│       └── CMakeLists.txt
├── Resources/                  # UI definitions and resources
└── build/                      # Build output directory
    ├── examples/               # Built example applications
    ├── tests/                  # Built test applications
    └── Resources/              # Copied resources
```

## Build Options

The build system supports the following options:

- `BUILD_EXAMPLES` (ON/OFF) - Build example applications
- `BUILD_TESTS` (ON/OFF) - Build test applications
- `BUILD_SHARED_LIBS` (ON/OFF) - Build shared libraries instead of static
- `CMAKE_BUILD_TYPE` (Debug/Release) - Build configuration

## Quick Start

### Windows

Use the provided build script:

```bat
# Release build with all features
build.bat

# Debug build without examples
build.bat --debug --no-examples

# Clean build with verbose output
build.bat --clean --verbose
```

### Manual CMake Build

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON

# Build
cmake --build . --config Release

# Run tests
ctest --config Release --output-on-failure
```

## Libraries

### DeclarativeUI (Main Library)

- **Core**: UIElement, DeclarativeBuilder
- **JSON**: JSONUILoader, JSONParser, JSONValidator, ComponentRegistry
- **HotReload**: FileWatcher, HotReloadManager, PerformanceMonitor
- **Binding**: StateManager, PropertyBinding
- **Command**: CommandSystem, BuiltinCommands, CommandIntegration

### Components Library

- **Input**: Button, LineEdit, CheckBox, ComboBox, SpinBox, Slider
- **Display**: Label, ProgressBar, TextEdit
- **Container**: Container, Layout, GroupBox, TabWidget
- **Advanced**: TableView, TreeView, MenuBar, DateTimeEdit

## Examples

All examples are built to `build/examples/` directory:

- **BasicExample**: Basic UI creation
- **HotReloadExample**: Hot reload functionality
- **FormExample**: Form components demonstration
- **DashboardExample**: Dashboard layout example
- **SettingsExample**: Settings dialog example
- **ComprehensiveExample**: All components showcase
- **AdvancedExample**: Advanced features demonstration
- **CommandExample**: Command system usage

## Tests

Tests are built to `build/tests/` directory:

- **CommandSystemTest**: Command system unit tests

## Dependencies

- Qt6 (Core, Widgets, Network, Test)
- CMake 3.20 or higher
- C++20 compatible compiler

## Development

### Adding New Components

1. Create `.hpp` and `.cpp` files in `src/Components/`
2. Add files to `src/Components/CMakeLists.txt`
3. Update component registry if needed

### Adding New Examples

1. Create `.cpp` file in `src/Examples/`
2. Add executable to `src/Examples/CMakeLists.txt`
3. Link against required libraries

### Adding New Tests

1. Create test file in `src/Tests/`
2. Add executable to `src/Tests/CMakeLists.txt`
3. Add test to CTest configuration

## Troubleshooting

### Common Issues

1. **Qt6 not found**: Ensure Qt6 is installed and in PATH
2. **Build failures**: Check compiler C++20 support
3. **Resource loading**: Verify Resources directory is copied to build output

### Debug Build

For debugging, use:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DDECLARATIVE_UI_DEBUG=ON
```

This enables additional debug output and assertions.
