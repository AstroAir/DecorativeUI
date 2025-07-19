@echo off
REM **DeclarativeUI Build Script for Windows**
REM This script provides easy build options for the DeclarativeUI project

setlocal EnableDelayedExpansion

REM **Configuration options**
set BUILD_TYPE=Release
set BUILD_EXAMPLES=ON
set BUILD_TESTS=ON
set CLEAN_BUILD=OFF
set VERBOSE=OFF

REM **Parse command line arguments**
:parse_args
if "%~1"=="" goto :done_parsing
if "%~1"=="--debug" (
    set BUILD_TYPE=Debug
    shift
    goto :parse_args
)
if "%~1"=="--release" (
    set BUILD_TYPE=Release
    shift
    goto :parse_args
)
if "%~1"=="--no-examples" (
    set BUILD_EXAMPLES=OFF
    shift
    goto :parse_args
)
if "%~1"=="--no-tests" (
    set BUILD_TESTS=OFF
    shift
    goto :parse_args
)
if "%~1"=="--clean" (
    set CLEAN_BUILD=ON
    shift
    goto :parse_args
)
if "%~1"=="--verbose" (
    set VERBOSE=ON
    shift
    goto :parse_args
)
if "%~1"=="--help" (
    goto :show_help
)
echo Unknown argument: %~1
goto :show_help

:done_parsing

echo **DeclarativeUI Build Configuration**
echo Build Type: %BUILD_TYPE%
echo Build Examples: %BUILD_EXAMPLES%
echo Build Tests: %BUILD_TESTS%
echo Clean Build: %CLEAN_BUILD%
echo.

REM **Clean build directory if requested**
if "%CLEAN_BUILD%"=="ON" (
    echo Cleaning build directory...
    if exist build rmdir /s /q build
)

REM **Create build directory**
if not exist build mkdir build
cd build

REM **Configure CMake**
echo Configuring CMake...
set CMAKE_CMD=cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DBUILD_EXAMPLES=%BUILD_EXAMPLES% -DBUILD_TESTS=%BUILD_TESTS%

if "%VERBOSE%"=="ON" (
    set CMAKE_CMD=%CMAKE_CMD% -DCMAKE_VERBOSE_MAKEFILE=ON
)

%CMAKE_CMD%
if errorlevel 1 (
    echo CMake configuration failed!
    goto :error
)

REM **Build the project**
echo Building project...
set BUILD_CMD=cmake --build . --config %BUILD_TYPE%

if "%VERBOSE%"=="ON" (
    set BUILD_CMD=%BUILD_CMD% --verbose
)

%BUILD_CMD%
if errorlevel 1 (
    echo Build failed!
    goto :error
)

echo.
echo **Build completed successfully!**
echo.
echo **Output locations:**
echo - Library: build/libDeclarativeUI.a
echo - Examples: build/examples/
echo - Tests: build/tests/
echo.

if "%BUILD_TESTS%"=="ON" (
    echo **Running tests...**
    ctest --config %BUILD_TYPE% --output-on-failure
)

goto :end

:show_help
echo **DeclarativeUI Build Script**
echo.
echo Usage: build.bat [options]
echo.
echo Options:
echo   --debug         Build in Debug mode (default: Release)
echo   --release       Build in Release mode
echo   --no-examples   Don't build examples
echo   --no-tests      Don't build tests
echo   --clean         Clean build directory before building
echo   --verbose       Enable verbose output
echo   --help          Show this help message
echo.
goto :end

:error
echo Build failed with error code %errorlevel%
cd ..
exit /b %errorlevel%

:end
cd ..
