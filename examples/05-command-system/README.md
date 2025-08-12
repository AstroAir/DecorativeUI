# Command System Examples (Experimental)

Command-based architecture demonstrations for DeclarativeUI applications.

⚠️ **Note:** The command system is currently experimental. Some examples may be disabled due to API changes and are being updated to align with the current DeclarativeUI architecture.

## Prerequisites

- Completed all previous example categories
- Strong understanding of design patterns
- Experience with command pattern and MVVM architectures
- Familiarity with JSON-driven UI concepts

## Examples in this Category

### Command Basics (`command-basics/`)
**Difficulty:** ⭐⭐⭐☆☆  
**Concepts:** Command pattern, basic command operations, undo/redo

Introduction to the command system and basic command operations.

**Examples included:**
- `command-basics.cpp` - Core command pattern implementation
- `command-basics-simple.cpp` - Simplified command examples

**What you'll learn:**
- Command pattern fundamentals
- Creating and executing commands
- Basic undo/redo functionality
- Command composition
- Command validation

**Run these examples:**
```bash
cmake --build . --target CommandBasicsExample
./examples/05-command-system/CommandBasicsExample

cmake --build . --target CommandBasicsSimpleExample
./examples/05-command-system/CommandBasicsSimpleExample
```

### Command Builder (`command-builder/`) 🚧
**Difficulty:** ⭐⭐⭐⭐☆  
**Concepts:** Builder pattern, fluent interfaces, command composition

**Status:** Currently disabled - being updated for API compatibility

Advanced command construction using builder patterns and fluent interfaces.

**What you'll learn (when re-enabled):**
- Command builder patterns
- Fluent interface design
- Complex command composition
- Command validation and error handling
- Performance optimization for command chains

### JSON Commands (`json-commands/`) 🚧
**Difficulty:** ⭐⭐⭐⭐☆  
**Concepts:** JSON-driven commands, dynamic command creation

**Status:** Currently disabled - being updated for API compatibility

Creating and executing commands from JSON definitions.

**What you'll learn (when re-enabled):**
- JSON command schemas
- Dynamic command creation
- Command serialization/deserialization
- External command configuration
- Command versioning and migration

### State Integration (`state-integration/`) 🚧
**Difficulty:** ⭐⭐⭐⭐⭐☆  
**Concepts:** Command-state integration, reactive commands, state synchronization

**Status:** Currently disabled - being updated for API compatibility

Integration between command system and reactive state management.

**What you'll learn (when re-enabled):**
- Command-driven state changes
- Reactive command execution
- State synchronization patterns
- Command-based data flow
- Advanced undo/redo with state

## Learning Path

### Currently Available
1. **Start with:** `command-basics/command-basics-simple.cpp` - Simple command concepts
2. **Then:** `command-basics/command-basics.cpp` - Full command pattern

### When Re-enabled
3. **Advance to:** `command-builder/` - Builder patterns
4. **Explore:** `json-commands/` - Dynamic commands
5. **Master:** `state-integration/` - Advanced integration

## Command System Architecture

### Core Concepts

**Command Pattern**
- Encapsulates requests as objects
- Enables parameterization and queuing
- Supports undo/redo operations
- Decouples invoker from receiver

**Command Types**
- **Simple Commands** - Single operations
- **Composite Commands** - Multiple operations
- **Macro Commands** - Recorded command sequences
- **Conditional Commands** - Context-dependent execution

**Command Lifecycle**
1. **Creation** - Command instantiation
2. **Validation** - Pre-execution checks
3. **Execution** - Command operation
4. **Logging** - Operation recording
5. **Cleanup** - Resource management

### Benefits

**Flexibility**
- Dynamic command creation
- Runtime command composition
- Configurable command behavior
- Extensible command system

**Maintainability**
- Clear separation of concerns
- Testable command logic
- Reusable command components
- Consistent error handling

**User Experience**
- Undo/redo functionality
- Command history
- Batch operations
- Macro recording

## Best Practices

### Command Design
1. **Keep commands focused** - Single responsibility principle
2. **Make commands immutable** - Avoid side effects
3. **Validate early** - Check preconditions before execution
4. **Handle errors gracefully** - Provide meaningful error messages
5. **Document command contracts** - Clear input/output specifications

### Performance
1. **Lazy execution** - Defer expensive operations
2. **Command pooling** - Reuse command objects
3. **Batch processing** - Group related commands
4. **Memory management** - Clean up command history

### Error Handling
1. **Validation** - Check command validity before execution
2. **Rollback** - Implement proper undo mechanisms
3. **Error reporting** - Provide detailed error information
4. **Recovery** - Handle partial failures gracefully

## Common Patterns

### Basic Command
```cpp
class IncrementCommand : public Command {
public:
    IncrementCommand(int& value) : m_value(value), m_oldValue(value) {}
    
    void execute() override {
        m_value++;
    }
    
    void undo() override {
        m_value = m_oldValue;
    }
    
private:
    int& m_value;
    int m_oldValue;
};
```

### Command Builder (when available)
```cpp
auto command = CommandBuilder()
    .setTarget(widget)
    .setProperty("text", "New Value")
    .setAnimation(true)
    .setDuration(300)
    .build();
```

### JSON Command (when available)
```json
{
    "type": "SetPropertyCommand",
    "target": "mainButton",
    "property": "text",
    "value": "Click Me!",
    "animation": {
        "duration": 200,
        "easing": "easeInOut"
    }
}
```

## Current Status

### Working Examples
- ✅ `command-basics/command-basics.cpp`
- ✅ `command-basics/command-basics-simple.cpp`

### Under Development
- 🚧 `command-builder/` - API alignment in progress
- 🚧 `json-commands/` - Schema updates needed
- 🚧 `state-integration/` - Integration refactoring

### Planned Features
- Command recording and playback
- Visual command editor
- Command performance profiling
- Advanced command composition tools

## Migration Notes

If you're updating from older command system examples:

1. **API Changes** - Some command interfaces have changed
2. **New Patterns** - Updated to use modern C++ patterns
3. **Performance** - Improved performance characteristics
4. **Integration** - Better integration with DeclarativeUI core

## Next Steps

After understanding the command system:
- **06-applications/** - See commands used in complete applications
- Explore creating custom command types
- Consider contributing to command system development
