# Command System Examples (Experimental)

Several command examples are currently disabled in CMake due to API mismatches. Review sources in this directory for reference; we plan to re-enable once APIs align.

## Planned run (when enabled)

```bash
cmake --preset command-dev
cmake --build --preset command-dev --target CommandUIExample IntegrationExample CommandBuilderExample JSONCommandExample StateIntegrationExample
```

## Contents

- CommandUIExample — Command-based UI
- IntegrationExample — Legacy/command integration
- CommandBuilderExample — Builder patterns
- JSONCommandExample — JSON loader with commands
- StateIntegrationExample — State + commands
