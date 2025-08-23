@echo off
REM Build Script for DeclarativeUI with Multi-Platform Support
REM This script provides configurable build options and platform detection

setlocal enabledelayedexpansion

REM **Default values**
set BUILD_TYPE=Release
set PRESET=default
set CLEAN_BUILD=false
set VERBOSE=false
set RUN_TESTS=false
set RUN_EXAMPLES=false
set ENABLE_LTO=false
set ENABLE_UNITY_BUILD=false
set ENABLE_CCACHE=true
set ENABLE_PCH=true
set TARGET_ARCH=x64

REM **Parse command line arguments**
:parse_args
if "%~1"=="" goto start_build
if /i "%~1"=="--help" goto show_help
if /i "%~1"=="-h" goto show_help
if /i "%~1"=="--preset" (
    set PRESET=%~2
    shift
    shift
    goto parse_args
)
if /i "%~1"=="--arch" (
    set TARGET_ARCH=%~2
    shift
    shift
    goto parse_args
)
if /i "%~1"=="--clean" (
    set CLEAN_BUILD=true
    shift
    goto parse_args
)
if /i "%~1"=="--verbose" (
    set VERBOSE=true
    shift
    goto parse_args
)
if /i "%~1"=="--test" (
    set RUN_TESTS=true
    shift
    goto parse_args
)
if /i "%~1"=="--examples" (
    set RUN_EXAMPLES=true
    shift
    goto parse_args
)
if /i "%~1"=="--lto" (
    set ENABLE_LTO=true
    shift
    goto parse_args
)
if /i "%~1"=="--unity" (
    set ENABLE_UNITY_BUILD=true
    shift
    goto parse_args
)
if /i "%~1"=="--no-ccache" (
    set ENABLE_CCACHE=false
    shift
    goto parse_args
)
if /i "%~1"=="--no-pch" (
    set ENABLE_PCH=false
    shift
    goto parse_args
)
echo Unknown argument: %~1
shift
goto parse_args

:show_help
echo Build Script for DeclarativeUI
echo.
echo Usage: configure.bat [options]
echo.
echo Options:
echo   --preset PRESET     CMake preset to use (default: default)
echo   --arch ARCH         Target architecture: x64, arm64 (default: x64)
echo   --clean             Clean build directory before building
echo   --verbose           Enable verbose build output
echo   --test              Run tests after building
echo   --examples          Run examples after building
echo   --lto               Enable Link Time Optimization
echo   --unity             Enable Unity builds for faster compilation
echo   --no-ccache         Disable ccache
echo   --no-pch            Disable precompiled headers
echo   -h, --help          Show this help message
echo.
echo Available presets:
echo   default             Standard release build
echo   debug               Debug build with diagnostics
echo   performance         High-performance build with LTO
echo   fast-build          Fast development build
echo   arm64-windows       ARM64 Windows build
echo.
echo Examples:
echo   configure.bat --preset performance --lto --unity
echo   configure.bat --arch arm64 --preset arm64-windows
echo   configure.bat --clean --test --verbose
goto :eof

:start_build
echo ========================================
echo DeclarativeUI Build Script
echo ========================================
echo Configuration:
echo   Preset: %PRESET%
echo   Architecture: %TARGET_ARCH%
echo   Clean Build: %CLEAN_BUILD%
echo   Verbose: %VERBOSE%
echo   Run Tests: %RUN_TESTS%
echo   Run Examples: %RUN_EXAMPLES%
echo   LTO: %ENABLE_LTO%
echo   Unity Build: %ENABLE_UNITY_BUILD%
echo   ccache: %ENABLE_CCACHE%
echo   PCH: %ENABLE_PCH%
echo.

REM **Check for CMake**
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake and add it to your PATH
    exit /b 1
)

REM **Check for Qt6**
echo Checking for Qt6...
where qmake >nul 2>&1
if errorlevel 1 (
    echo WARNING: qmake not found in PATH
    echo Make sure Qt6 is installed and in your PATH
)

REM **Detect platform and architecture**
echo Detecting platform and architecture...
if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
    set DETECTED_ARCH=arm64
) else (
    set DETECTED_ARCH=x64
)
echo Detected architecture: %DETECTED_ARCH%

REM **Clean build if requested**
if "%CLEAN_BUILD%"=="true" (
    echo Cleaning previous builds...
    if exist build rmdir /s /q build
    if exist build-* rmdir /s /q build-*
)

REM **Set additional CMake variables based on options**
set CMAKE_EXTRA_VARS=
if "%ENABLE_LTO%"=="true" (
    set CMAKE_EXTRA_VARS=%CMAKE_EXTRA_VARS% -DENABLE_LTO=ON
)
if "%ENABLE_UNITY_BUILD%"=="true" (
    set CMAKE_EXTRA_VARS=%CMAKE_EXTRA_VARS% -DENABLE_UNITY_BUILD=ON
)
if "%ENABLE_CCACHE%"=="true" (
    set CMAKE_EXTRA_VARS=%CMAKE_EXTRA_VARS% -DENABLE_CCACHE=ON
) else (
    set CMAKE_EXTRA_VARS=%CMAKE_EXTRA_VARS% -DENABLE_CCACHE=OFF
)
if "%ENABLE_PCH%"=="true" (
    set CMAKE_EXTRA_VARS=%CMAKE_EXTRA_VARS% -DENABLE_PRECOMPILED_HEADERS=ON
) else (
    set CMAKE_EXTRA_VARS=%CMAKE_EXTRA_VARS% -DENABLE_PRECOMPILED_HEADERS=OFF
)

REM **Configure build**
echo.
echo Configuring build with preset: %PRESET%
if "%CMAKE_EXTRA_VARS%"=="" (
    cmake --preset %PRESET%
) else (
    cmake --preset %PRESET% %CMAKE_EXTRA_VARS%
)
if errorlevel 1 (
    echo ERROR: Configuration failed
    exit /b 1
)

REM **Build**
echo.
echo Building...
if "%VERBOSE%"=="true" (
    cmake --build --preset %PRESET% --verbose
) else (
    cmake --build --preset %PRESET%
)
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

REM **Run tests if requested**
if "%RUN_TESTS%"=="true" (
    echo.
    echo Running tests...
    cd build* 2>nul || cd build
    ctest --output-on-failure --parallel 4
    if errorlevel 1 (
        echo WARNING: Some tests failed
    ) else (
        echo All tests passed!
    )
    cd ..
)

REM **Run examples if requested**
if "%RUN_EXAMPLES%"=="true" (
    echo.
    echo Running examples...
    cd build* 2>nul || cd build
    if exist examples\basic\HelloWorldExample.exe (
        echo Running HelloWorldExample...
        examples\basic\HelloWorldExample.exe
    )
    cd ..
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
goto :eof

:error
echo Build failed!
exit /b 1
