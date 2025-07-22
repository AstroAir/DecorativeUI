@echo off
REM **Command System Build Script for Windows**
REM This script builds the DeclarativeUI framework with Command system enabled

setlocal enabledelayedexpansion

echo.
echo ========================================
echo  DeclarativeUI Command System Builder
echo ========================================
echo.

REM **Default configuration**
set BUILD_TYPE=Debug
set PRESET=command-dev
set CLEAN_BUILD=false
set VERBOSE=false
set RUN_TESTS=false
set RUN_EXAMPLES=false

REM **Parse command line arguments**
:parse_args
if "%~1"=="" goto :start_build
if /i "%~1"=="--release" (
    set BUILD_TYPE=Release
    set PRESET=default
    shift
    goto :parse_args
)
if /i "%~1"=="--debug" (
    set BUILD_TYPE=Debug
    set PRESET=command-dev
    shift
    goto :parse_args
)
if /i "%~1"=="--legacy-only" (
    set PRESET=legacy-only
    shift
    goto :parse_args
)
if /i "%~1"=="--minimal" (
    set PRESET=minimal
    shift
    goto :parse_args
)
if /i "%~1"=="--clean" (
    set CLEAN_BUILD=true
    shift
    goto :parse_args
)
if /i "%~1"=="--verbose" (
    set VERBOSE=true
    shift
    goto :parse_args
)
if /i "%~1"=="--test" (
    set RUN_TESTS=true
    shift
    goto :parse_args
)
if /i "%~1"=="--examples" (
    set RUN_EXAMPLES=true
    shift
    goto :parse_args
)
if /i "%~1"=="--help" (
    goto :show_help
)
echo Unknown argument: %~1
shift
goto :parse_args

:start_build
echo Configuration:
echo   Build Type: %BUILD_TYPE%
echo   Preset: %PRESET%
echo   Clean Build: %CLEAN_BUILD%
echo   Verbose: %VERBOSE%
echo   Run Tests: %RUN_TESTS%
echo   Run Examples: %RUN_EXAMPLES%
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

REM **Clean build if requested**
if "%CLEAN_BUILD%"=="true" (
    echo Cleaning previous build...
    if exist build rmdir /s /q build
    if exist build-command rmdir /s /q build-command
    if exist build-legacy rmdir /s /q build-legacy
    if exist build-minimal rmdir /s /q build-minimal
)

REM **Configure build**
echo.
echo Configuring build with preset: %PRESET%
cmake --preset %PRESET%
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

echo.
echo ✅ Build completed successfully!

REM **Run tests if requested**
if "%RUN_TESTS%"=="true" (
    echo.
    echo Running tests...
    ctest --preset %PRESET% --output-on-failure
    if errorlevel 1 (
        echo WARNING: Some tests failed
    ) else (
        echo ✅ All tests passed!
    )
)

REM **Run examples if requested**
if "%RUN_EXAMPLES%"=="true" (
    echo.
    echo Available examples in build/examples/:
    if exist build\examples (
        dir /b build\examples\*.exe
    )
    if exist build-command\examples (
        dir /b build-command\examples\*.exe
    )
)

REM **Show build summary**
echo.
echo ========================================
echo  Build Summary
echo ========================================
echo   Preset: %PRESET%
echo   Build Type: %BUILD_TYPE%
if "%PRESET%"=="command-dev" (
    echo   Command System: ✅ Enabled with debug
    echo   Adapters: ✅ Enabled
    echo   Command Examples: ✅ Enabled
    echo   Command Tests: ✅ Enabled
) else if "%PRESET%"=="default" (
    echo   Command System: ✅ Enabled
    echo   Adapters: ✅ Enabled
    echo   Command Examples: ✅ Enabled
    echo   Command Tests: ✅ Enabled
) else if "%PRESET%"=="legacy-only" (
    echo   Command System: ❌ Disabled
    echo   Adapters: ❌ Disabled
    echo   Legacy Components: ✅ Enabled
) else if "%PRESET%"=="minimal" (
    echo   Command System: ❌ Disabled
    echo   Examples: ❌ Disabled
    echo   Tests: ❌ Disabled
)

echo.
echo Build artifacts:
if exist build (
    echo   - build/examples/ - Example applications
    echo   - build/tests/ - Test applications
)
if exist build-command (
    echo   - build-command/examples/ - Command system examples
    echo   - build-command/tests/ - Command system tests
)

echo.
echo To run Command system examples:
if "%PRESET%"=="command-dev" (
    echo   build-command\examples\CommandBuilderExample.exe
    echo   build-command\examples\StateIntegrationExample.exe
    echo   build-command\examples\JSONCommandExample.exe
) else if "%PRESET%"=="default" (
    echo   build\examples\CommandBuilderExample.exe
    echo   build\examples\StateIntegrationExample.exe
    echo   build\examples\JSONCommandExample.exe
)

goto :end

:show_help
echo.
echo DeclarativeUI Command System Build Script
echo.
echo Usage: build-command.bat [options]
echo.
echo Options:
echo   --release        Build in Release mode (default preset)
echo   --debug          Build in Debug mode (command-dev preset)
echo   --legacy-only    Build only legacy components
echo   --minimal        Build minimal library only
echo   --clean          Clean previous build
echo   --verbose        Verbose build output
echo   --test           Run tests after build
echo   --examples       List available examples after build
echo   --help           Show this help
echo.
echo Presets:
echo   command-dev      Command system development (Debug + all features)
echo   default          Release with Command system
echo   legacy-only      Legacy components only
echo   minimal          Library only
echo.
echo Examples:
echo   build-command.bat --debug --test
echo   build-command.bat --release --clean
echo   build-command.bat --legacy-only --examples
echo.

:end
endlocal
