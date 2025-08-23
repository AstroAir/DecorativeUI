# Unit Tests

Purpose
- Validate functionality, performance, and stability of DeclarativeUI modules

Conventions
- Each test file focuses on a feature area; avoid cross-coupled assertions
- Prefer fast, deterministic tests; isolate Qt event loop usage
- Clean up created QObject/QWidget instances; prefer smart pointers

Recent updates
- AccessibilityTest: initialized QApplication* to nullptr to avoid deleting an uninitialized pointer in cleanup
- Component/Frame/Label/ScrollArea behavior adjusted; tests reflect lifecycle and property application order
- PropertyBindingTemplate: safer widget lifetime using QPointer

How to run
- From build directory: ctest --output-on-failure -j 4
- To run a suite: ctest -R PropertyBindingTemplateTest -VV
