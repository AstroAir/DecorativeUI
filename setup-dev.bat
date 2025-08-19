@echo off
setlocal enabledelayedexpansion

REM DeclarativeUI Development Setup Script for Windows
REM This script sets up the development environment including pre-commit hooks

echo 🚀 Setting up DeclarativeUI development environment...

REM Check if Python is available
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Python 3 is required but not installed. Please install Python 3.8+ and try again.
    pause
    exit /b 1
)

echo ✅ Found Python:
python --version

REM Check if pip is available
python -m pip --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ pip is required but not available. Please install pip and try again.
    pause
    exit /b 1
)

REM Install Python dependencies
echo 📦 Installing Python dependencies...
python -m pip install -r requirements.txt
if %errorlevel% neq 0 (
    echo ❌ Failed to install Python dependencies
    pause
    exit /b 1
)

REM Install pre-commit hooks
echo 🔧 Installing pre-commit hooks...
pre-commit install
if %errorlevel% neq 0 (
    echo ❌ Failed to install pre-commit hooks
    pause
    exit /b 1
)

REM Run pre-commit on all files to check current state
echo 🔍 Running pre-commit on all files (this may take a while on first run)...
pre-commit run --all-files
if %errorlevel% neq 0 (
    echo ⚠️  Some pre-commit checks failed. This is normal for the initial setup.
    echo    The hooks will automatically fix most issues on your next commit.
) else (
    echo ✅ All pre-commit checks passed!
)

echo.
echo 🎉 Development environment setup complete!
echo.
echo 📋 Next steps:
echo    1. Create a build directory and configure CMake:
echo       mkdir build && cd build
echo       cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON
echo.
echo    2. Build the project:
echo       cmake --build .
echo.
echo    3. Run tests to verify setup:
echo       ctest --output-on-failure
echo.
echo 💡 Tips:
echo    - Pre-commit hooks will run automatically on each commit
echo    - To skip hooks in emergencies: git commit --no-verify
echo    - To run hooks manually: pre-commit run --all-files
echo    - To update hooks: pre-commit autoupdate

pause
