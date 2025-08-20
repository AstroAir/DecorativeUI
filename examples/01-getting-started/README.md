# Getting Started Examples

Perfect for absolute beginners to DeclarativeUI. These examples introduce the most basic concepts with minimal complexity.

## Prerequisites

- Basic C++ knowledge
- Qt6 installed
- DeclarativeUI library built

## Examples in this Category

### Hello World (`hello-world/`)

**Difficulty:** ⭐☆☆☆☆
**Concepts:** Basic window creation, minimal setup

The simplest possible DeclarativeUI application. Creates a basic window with a "Hello World" message.

**What you'll learn:**

- How to set up a basic DeclarativeUI application
- Creating your first window
- Basic application structure

**Run this example:**

```bash
cmake --build . --target HelloWorldExample
./examples/01-getting-started/HelloWorldExample
```

### Basic UI (`basic-ui/`)

**Difficulty:** ⭐⭐☆☆☆
**Concepts:** Basic components, simple layouts

Introduction to basic UI components like buttons, labels, and simple layouts.

**What you'll learn:**

- Using basic UI components (Button, Label)
- Simple layout management
- Component interaction basics

**Run this example:**

```bash
cmake --build . --target BasicUIExample
./examples/01-getting-started/BasicUIExample
```

### Simple Events (`simple-events/`)

**Difficulty:** ⭐⭐☆☆☆
**Concepts:** Event handling, signals and slots

Basic event handling patterns using Qt's signals and slots mechanism.

**What you'll learn:**

- Connecting signals to slots
- Handling button clicks
- Basic event-driven programming

**Run this example:**

```bash
cmake --build . --target SimpleEventsExample
./examples/01-getting-started/SimpleEventsExample
```

## Learning Path

1. **Start here:** `hello-world/` - Get familiar with the basic structure
2. **Next:** `basic-ui/` - Learn about UI components
3. **Then:** `simple-events/` - Understand event handling

## Next Steps

After completing these examples, move on to:

- **02-fundamentals/** - Learn core concepts like state management
- **03-components/** - Explore more UI components

## Common Issues

### Build Errors

- Ensure Qt6 is properly installed
- Check that DeclarativeUI library is built
- Verify CMake configuration

### Runtime Errors

- Make sure all required libraries are in your PATH
- Check that resources are copied to build directory

## Tips for Beginners

1. **Read the code comments** - Each example is heavily commented
2. **Experiment** - Try modifying the examples to see what happens
3. **Build incrementally** - Start with hello-world and work your way up
4. **Ask questions** - Don't hesitate to seek help if you're stuck

## Code Style

These examples follow these conventions:

- Use `std::unique_ptr` for widget ownership
- Clear, descriptive variable names
- Extensive comments explaining each step
- Simple, linear code flow
