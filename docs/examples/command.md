# Command System Examples (Status: Experimental)

These examples highlight the command-based UI architecture.

Currently, several command examples are disabled in CMake due to API mismatches. You can still review the source files in `examples/command/`. We plan to re-enable them after aligning APIs.

## Running (when enabled)

```bash
cmake --preset command-dev
cmake --build --preset command-dev --target CommandUIExample IntegrationExample CommandBuilderExample JSONCommandExample StateIntegrationExample
```

## Examples (planned)

- CommandUIExample — Constructing UI via command system
- IntegrationExample — Integrating command UI with legacy components
- CommandBuilderExample — Command builder patterns
- JSONCommandExample — Loading UI from JSON using command adapters
- StateIntegrationExample — Command-state integration patterns
