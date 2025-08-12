# Contributing to DeclarativeUI

Thank you for your interest in contributing to DeclarativeUI! We welcome contributions from the community and are excited to see what you'll build with us.

## Quick Start

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/DeclarativeUI.git
   cd DeclarativeUI
   ```
3. **Set up development environment** (see [Development Setup](#development-setup))
4. **Create a feature branch**: `git checkout -b feature/your-feature-name`
5. **Make your changes** following our [coding standards](#coding-standards)
6. **Write tests** for your changes
7. **Submit a pull request**

## Development Setup

### Prerequisites

- **Qt6** (6.2 or later) with Core, Widgets, Network, and Test modules
- **CMake** 3.20 or higher
- **C++20 compatible compiler** (GCC 10+, Clang 12+, MSVC 2019+)
- **Git**

### Build Instructions

```bash
# Create development build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON
cmake --build .

# Run tests to verify setup
ctest --output-on-failure
```

For detailed setup instructions, see our [Development Guide](docs/developer/contributing.md).

## How to Contribute

### Reporting Issues

- Use GitHub Issues to report bugs or request features
- Search existing issues before creating new ones
- Provide detailed reproduction steps for bugs
- Include system information (OS, Qt version, compiler)

### Submitting Changes

1. **Small changes**: Feel free to submit a PR directly
2. **Large changes**: Please open an issue first to discuss the approach
3. **Breaking changes**: Must be discussed in an issue before implementation

### Types of Contributions

- 🐛 **Bug fixes**: Fix issues in existing functionality
- ✨ **New features**: Add new components or capabilities
- 📚 **Documentation**: Improve docs, examples, or guides
- 🧪 **Tests**: Add or improve test coverage
- 🎨 **Examples**: Create new example applications
- ⚡ **Performance**: Optimize existing code

## Coding Standards

We follow modern C++20 best practices:

- **Naming**: PascalCase for classes, camelCase for methods, snake_case for variables
- **Memory Management**: Use smart pointers and RAII
- **Error Handling**: Use exceptions for error conditions
- **Documentation**: Document public APIs with Doxygen comments

For detailed coding standards, see [docs/developer/contributing.md](docs/developer/contributing.md).

## Testing

- Write unit tests for all new functionality
- Ensure existing tests continue to pass
- Add integration tests for complex features
- Include performance tests for critical paths

## Documentation

- Update relevant documentation for your changes
- Add examples for new features
- Update API documentation for public interfaces
- Consider adding tutorials for complex features

## Code Review Process

1. All submissions require review from a maintainer
2. Automated checks must pass (CI, tests, linting)
3. Address all feedback before merge
4. Squash commits if requested

## Community Guidelines

- Be respectful and inclusive
- Help others learn and grow
- Share knowledge and best practices
- Celebrate successes and learn from failures

## Getting Help

- 📖 **Documentation**: Check [docs/](docs/) for comprehensive guides
- 💬 **Discussions**: Use GitHub Discussions for questions
- 🐛 **Issues**: Report bugs or request features
- 📧 **Contact**: Reach out to maintainers for complex questions

## Recognition

Contributors are recognized in:
- CHANGELOG.md for their contributions
- README.md contributors section
- Release notes for significant contributions

Thank you for helping make DeclarativeUI better! 🚀

---

For detailed development guidelines, see [docs/developer/contributing.md](docs/developer/contributing.md).
