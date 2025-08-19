# Pre-commit Hooks Setup Guide

This document provides detailed instructions for setting up and using pre-commit hooks in the DeclarativeUI project.

## Overview

Pre-commit hooks help maintain code quality by automatically checking and fixing common issues before commits are made. Our setup includes:

- **Code Formatting**: Automatic C++ code formatting with clang-format
- **CMake Formatting**: CMakeLists.txt and .cmake file formatting
- **Static Analysis**: Basic C++ static analysis with cppcheck
- **General Checks**: Trailing whitespace, file size limits, merge conflicts
- **Documentation**: Markdown formatting and validation

## Quick Setup

### Prerequisites

- Python 3.8 or later
- Git
- C++ development tools (clang-format, cppcheck)

### Installation

```bash
# 1. Install Python dependencies
pip install -r requirements.txt

# 2. Install pre-commit hooks
pre-commit install

# 3. (Optional) Run on all files to check current state
pre-commit run --all-files
```

### Automated Setup

Use our setup scripts for automated installation:

**Linux/macOS:**

```bash
./setup-dev.sh
```

**Windows:**

```batch
setup-dev.bat
```

## Configuration

### Main Configuration (`.pre-commit-config.yaml`)

Our main configuration includes:

#### Code Formatting Hooks

- **clang-format**: Formats C++ code according to `.clang-format`
- **cmake-format**: Formats CMake files according to `.cmake-format.yaml`
- **black**: Formats Python scripts (build tools)

#### Quality Checks

- **cppcheck**: Static analysis for C++ code
- **trailing-whitespace**: Removes trailing whitespace
- **end-of-file-fixer**: Ensures files end with newline
- **check-large-files**: Prevents committing large files (>500KB)

#### File Validation

- **check-yaml**: Validates YAML syntax
- **check-json**: Validates JSON syntax
- **check-xml**: Validates XML syntax
- **check-merge-conflict**: Detects merge conflict markers

#### Branch Protection

- **no-commit-to-branch**: Prevents direct commits to main/master

### File Exclusions

The hooks automatically exclude:

- Build artifacts (`build/`, `*_autogen/`)
- Generated Qt files (`*.moc`, `moc_*.cpp`, `ui_*.h`)
- Test files (for some hooks like cppcheck)

### Customization

#### Clang-format Configuration

The project uses a Google-based clang-format style with modifications. See `.clang-format` for details.

#### CMake Format Configuration

CMake files are formatted with specific rules. See `.cmake-format.yaml` for configuration.

## Usage

### Automatic Execution

Pre-commit hooks run automatically on every `git commit`. If any hook fails:

1. The commit is aborted
2. Issues are fixed automatically (when possible)
3. You need to review changes and commit again

### Manual Execution

```bash
# Run hooks on all files
pre-commit run --all-files

# Run hooks on specific files
pre-commit run --files src/Core/MyFile.cpp src/Core/MyFile.hpp

# Run specific hook
pre-commit run clang-format

# Run hooks on staged files only
pre-commit run
```

### Bypassing Hooks

Sometimes you may need to bypass hooks:

```bash
# Skip all hooks (emergency commits only)
git commit --no-verify -m "Emergency fix"

# Skip specific hooks
SKIP=clang-format,cppcheck git commit -m "Skip formatting and analysis"

# Skip hooks for specific files
git commit -m "Update docs" -- README.md --no-verify
```

### Updating Hooks

```bash
# Update to latest versions
pre-commit autoupdate

# Update specific repository
pre-commit autoupdate --repo https://github.com/pre-commit/mirrors-clang-format
```

## Troubleshooting

### Common Issues

#### Network Connectivity

If you have network issues when installing hooks:

1. Check your internet connection
2. Try using a VPN if behind corporate firewall
3. Use the local configuration for testing: `cp .pre-commit-config-local.yaml .pre-commit-config.yaml`

#### Missing Tools

If clang-format or cppcheck are not found:

**Linux:**

```bash
# Ubuntu/Debian
sudo apt-get install clang-format cppcheck

# Fedora/RHEL
sudo dnf install clang-tools-extra cppcheck
```

**macOS:**

```bash
brew install clang-format cppcheck
```

**Windows:**

- Install LLVM (includes clang-format): <https://llvm.org/builds/>
- Install cppcheck: <https://cppcheck.sourceforge.io/>
- Ensure tools are in PATH

#### Performance Issues

If hooks are slow:

1. Exclude large directories in `.pre-commit-config.yaml`
2. Skip expensive hooks in CI: see the `ci.skip` configuration
3. Use `--hook-stage manual` for expensive hooks

#### Hook Failures

If a hook consistently fails:

1. Check the specific error message
2. Run the hook manually with verbose output: `pre-commit run --verbose hook-name`
3. Fix the underlying issue or adjust configuration
4. Consider disabling problematic hooks temporarily

### Getting Help

1. Check the [pre-commit documentation](https://pre-commit.com/)
2. Review our [Contributing Guide](CONTRIBUTING.md)
3. Open an issue in the project repository
4. Check existing issues for similar problems

## Integration with Development Workflow

### IDE Integration

Most IDEs can be configured to run pre-commit hooks:

**VS Code:**

- Install the "Pre-commit" extension
- Configure automatic formatting on save

**CLion/IntelliJ:**

- Use the "Pre-commit Hook Plugin"
- Configure external tools to run pre-commit

### CI/CD Integration

Pre-commit hooks can run in CI/CD pipelines:

```yaml
# GitHub Actions example
- name: Run pre-commit
  uses: pre-commit/action@v3.0.1
```

### Custom Hooks

You can add project-specific hooks by editing `.pre-commit-config.yaml`:

```yaml
- repo: local
  hooks:
    - id: my-custom-check
      name: Custom project check
      entry: scripts/custom-check.sh
      language: script
      files: \.(cpp|hpp)$
```

## Best Practices

1. **Install hooks early**: Set up pre-commit hooks as soon as you clone the repository
2. **Run periodically**: Use `pre-commit run --all-files` occasionally to catch issues
3. **Keep updated**: Run `pre-commit autoupdate` regularly
4. **Understand failures**: Don't just bypass hooks; understand why they fail
5. **Customize carefully**: Only modify configurations if you understand the implications
6. **Test changes**: Test hook configuration changes thoroughly before committing

## Performance Tips

1. **Exclude unnecessary files**: Add appropriate exclusions to reduce processing time
2. **Use file filters**: Configure hooks to run only on relevant file types
3. **Skip in CI**: Skip expensive hooks in CI environments where appropriate
4. **Cache dependencies**: Pre-commit caches tools automatically, don't clear unless necessary

## Security Considerations

1. **Review hook sources**: Only use hooks from trusted repositories
2. **Pin versions**: Use specific versions rather than latest to ensure reproducibility
3. **Audit changes**: Review updates to hook configurations carefully
4. **Local configuration**: Keep sensitive configurations in local files, not version control

---

For more information, see the main [Contributing Guide](CONTRIBUTING.md) or the [project documentation](docs/).
