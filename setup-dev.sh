#!/bin/bash

# DeclarativeUI Development Setup Script
# This script sets up the development environment including pre-commit hooks

set -e

echo "🚀 Setting up DeclarativeUI development environment..."

# Check if Python is available
if ! command -v python3 &> /dev/null && ! command -v python &> /dev/null; then
    echo "❌ Python 3 is required but not installed. Please install Python 3.8+ and try again."
    exit 1
fi

# Use python3 if available, otherwise python
PYTHON_CMD="python3"
if ! command -v python3 &> /dev/null; then
    PYTHON_CMD="python"
fi

echo "✅ Found Python: $($PYTHON_CMD --version)"

# Check if pip is available
if ! $PYTHON_CMD -m pip --version &> /dev/null; then
    echo "❌ pip is required but not available. Please install pip and try again."
    exit 1
fi

# Install Python dependencies
echo "📦 Installing Python dependencies..."
$PYTHON_CMD -m pip install -r requirements.txt

# Install pre-commit hooks
echo "🔧 Installing pre-commit hooks..."
pre-commit install

# Run pre-commit on all files to check current state
echo "🔍 Running pre-commit on all files (this may take a while on first run)..."
if pre-commit run --all-files; then
    echo "✅ All pre-commit checks passed!"
else
    echo "⚠️  Some pre-commit checks failed. This is normal for the initial setup."
    echo "   The hooks will automatically fix most issues on your next commit."
fi

echo ""
echo "🎉 Development environment setup complete!"
echo ""
echo "📋 Next steps:"
echo "   1. Create a build directory and configure CMake:"
echo "      mkdir build && cd build"
echo "      cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON"
echo ""
echo "   2. Build the project:"
echo "      cmake --build ."
echo ""
echo "   3. Run tests to verify setup:"
echo "      ctest --output-on-failure"
echo ""
echo "💡 Tips:"
echo "   - Pre-commit hooks will run automatically on each commit"
echo "   - To skip hooks in emergencies: git commit --no-verify"
echo "   - To run hooks manually: pre-commit run --all-files"
echo "   - To update hooks: pre-commit autoupdate"
