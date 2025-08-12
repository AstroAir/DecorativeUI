# Example Templates

This directory contains templates for creating new DeclarativeUI examples. These templates provide a standardized starting point and ensure consistency across all examples.

## Available Templates

### Basic Example Template (`basic-example-template/`)

**Use for:** Simple examples demonstrating specific concepts or features

**Includes:**
- `example-template.cpp` - Basic example structure
- `README.md` - Documentation template

**Best for:**
- Single concept demonstrations
- Component showcases
- Feature introductions
- Tutorial-style examples

### Application Template (`application-template/`)

**Use for:** Complete applications demonstrating real-world usage

**Includes:**
- `application-template.cpp` - Full application structure with menus, status bar, etc.
- `README.md` - Comprehensive application documentation template

**Best for:**
- Complete sample applications
- Complex feature demonstrations
- Architecture pattern examples
- Production-like examples

## Using Templates

### Creating a Basic Example

1. **Copy the template:**
   ```bash
   cp -r templates/basic-example-template/ [category]/[example-name]/
   ```

2. **Rename files:**
   ```bash
   cd [category]/[example-name]/
   mv example-template.cpp [example-name].cpp
   ```

3. **Update placeholders:**
   - Replace `[Example Name]` with your actual example name
   - Replace `[Brief description]` with your example description
   - Update the key concepts list
   - Fill in the TODO sections

4. **Add to build system:**
   - Update the category's `CMakeLists.txt`
   - Add your example target

5. **Update documentation:**
   - Complete the README.md template
   - Update the category README.md to include your example

### Creating an Application Example

1. **Copy the template:**
   ```bash
   cp -r templates/application-template/ 06-applications/[app-name]/
   ```

2. **Rename files:**
   ```bash
   cd 06-applications/[app-name]/
   mv application-template.cpp [app-name].cpp
   ```

3. **Customize the application:**
   - Replace `[Application Name]` placeholders
   - Implement the main application logic
   - Add application-specific UI components
   - Implement menu actions and event handlers

4. **Add to build system:**
   - Update `06-applications/CMakeLists.txt`
   - Add your application target

5. **Create comprehensive documentation:**
   - Complete the README.md with full application details
   - Include screenshots if helpful
   - Document all features and usage instructions

## Template Guidelines

### Code Style
- Use modern C++ features (C++17 or later)
- Follow Qt naming conventions
- Use `std::unique_ptr` for widget ownership where appropriate
- Include comprehensive comments
- Handle errors gracefully

### Documentation Style
- Start with a clear, concise description
- List prerequisites and difficulty level
- Explain key concepts thoroughly
- Provide step-by-step instructions
- Include troubleshooting information

### File Organization
- Keep examples self-contained
- Use descriptive file names (kebab-case)
- Include all necessary resources
- Organize code logically

## Template Customization

### Adding New Templates

If you need a template for a specific type of example:

1. **Create template directory:**
   ```bash
   mkdir templates/[template-name]/
   ```

2. **Create template files:**
   - Source code template with placeholders
   - README.md template
   - Any additional files needed

3. **Document the template:**
   - Add description to this README
   - Explain when to use the template
   - Provide usage instructions

4. **Test the template:**
   - Create an example using the template
   - Verify all placeholders work correctly
   - Ensure the template produces working code

### Template Best Practices

1. **Use clear placeholders:**
   - Use `[Placeholder Name]` format
   - Make placeholders descriptive
   - Document all placeholders

2. **Provide comprehensive structure:**
   - Include all common elements
   - Add TODO comments for customization points
   - Provide example implementations

3. **Include documentation:**
   - Template usage instructions
   - Best practices guidance
   - Common pitfalls to avoid

4. **Keep templates updated:**
   - Update when APIs change
   - Incorporate new best practices
   - Fix issues as they're discovered

## Common Placeholders

### Code Templates
- `[Example Name]` - Human-readable example name
- `[Brief description]` - Short description of what the example does
- `[Concept 1]`, `[Concept 2]`, etc. - Key concepts demonstrated
- `[Feature 1]`, `[Feature 2]`, etc. - Application features
- `TODO:` comments - Areas requiring customization

### Documentation Templates
- `[Example Name]` - Example title
- `[Brief description]` - Example description
- `[Category name]` - Example category
- `[ExampleTargetName]` - CMake target name
- `[Learning objective N]` - What users will learn
- `[Concept N]` - Key concepts explained

## Validation Checklist

Before submitting a new example created from a template:

### Code Quality
- [ ] All placeholders replaced with actual content
- [ ] Code compiles without warnings
- [ ] Example runs correctly
- [ ] Error handling implemented
- [ ] Code follows project style guidelines

### Documentation
- [ ] README.md completed with all sections
- [ ] Prerequisites clearly stated
- [ ] Learning objectives defined
- [ ] Usage instructions provided
- [ ] Troubleshooting information included

### Integration
- [ ] Added to appropriate CMakeLists.txt
- [ ] Category README.md updated
- [ ] Example tested in build system
- [ ] All dependencies documented

### Testing
- [ ] Example tested on target platforms
- [ ] Edge cases considered
- [ ] Performance acceptable
- [ ] Memory leaks checked

## Getting Help

If you need help using these templates:

1. **Check existing examples** - See how others have used the templates
2. **Read the documentation** - Review category README files
3. **Ask questions** - Use GitHub issues for template-related questions
4. **Contribute improvements** - Submit PRs to improve templates
