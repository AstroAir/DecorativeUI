# DeclarativeUI Development Tasks
# Usage: python tasks.py <task_name>

import sys
import subprocess
import os
from pathlib import Path


def run_command(cmd, description):
    """Run a command and print its output."""
    print(f"🔧 {description}...")
    try:
        result = subprocess.run(
            cmd, shell=True, check=True, capture_output=True, text=True
        )
        if result.stdout:
            print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"❌ Error: {e}")
        if e.stdout:
            print(f"Output: {e.stdout}")
        if e.stderr:
            print(f"Error: {e.stderr}")
        return False


def setup_dev():
    """Set up development environment."""
    print("🚀 Setting up DeclarativeUI development environment...")

    # Install Python dependencies
    if not run_command(
        "pip install -r requirements.txt", "Installing Python dependencies"
    ):
        return False

    # Install pre-commit hooks
    if not run_command("pre-commit install", "Installing pre-commit hooks"):
        return False

    print("✅ Development environment setup complete!")
    print("\n📋 Next steps:")
    print("   1. mkdir build && cd build")
    print(
        "   2. cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON"
    )
    print("   3. cmake --build .")
    print("   4. ctest --output-on-failure")
    return True


def format_code():
    """Format all code using pre-commit hooks."""
    return run_command("pre-commit run --all-files", "Formatting all code")


def lint_code():
    """Run linting on all code."""
    return run_command("pre-commit run cppcheck --all-files", "Running C++ linting")


def check_code():
    """Run all code quality checks."""
    return run_command("pre-commit run --all-files", "Running all code quality checks")


def build_debug():
    """Build project in debug mode."""
    os.makedirs("build", exist_ok=True)
    os.chdir("build")

    if not run_command(
        "cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON",
        "Configuring CMake",
    ):
        return False

    return run_command("cmake --build .", "Building project")


def build_release():
    """Build project in release mode."""
    os.makedirs("build", exist_ok=True)
    os.chdir("build")

    if not run_command(
        "cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON",
        "Configuring CMake",
    ):
        return False

    return run_command("cmake --build .", "Building project")


def run_tests():
    """Run all tests."""
    if not Path("build").exists():
        print("❌ Build directory not found. Run 'python tasks.py build-debug' first.")
        return False

    os.chdir("build")
    return run_command("ctest --output-on-failure", "Running tests")


def clean():
    """Clean build artifacts."""
    import shutil

    if Path("build").exists():
        shutil.rmtree("build")
        print("🧹 Cleaned build directory")
    return True


def update_hooks():
    """Update pre-commit hooks to latest versions."""
    return run_command("pre-commit autoupdate", "Updating pre-commit hooks")


def help():
    """Show available tasks."""
    print("📋 Available tasks:")
    print("   setup-dev     - Set up development environment")
    print("   format        - Format all code")
    print("   lint          - Run C++ linting")
    print("   check         - Run all code quality checks")
    print("   build-debug   - Build project in debug mode")
    print("   build-release - Build project in release mode")
    print("   test          - Run all tests")
    print("   clean         - Clean build artifacts")
    print("   update-hooks  - Update pre-commit hooks")
    print("   help          - Show this help")


def main():
    if len(sys.argv) != 2:
        help()
        return

    task = sys.argv[1]

    tasks = {
        "setup-dev": setup_dev,
        "format": format_code,
        "lint": lint_code,
        "check": check_code,
        "build-debug": build_debug,
        "build-release": build_release,
        "test": run_tests,
        "clean": clean,
        "update-hooks": update_hooks,
        "help": help,
    }

    if task not in tasks:
        print(f"❌ Unknown task: {task}")
        help()
        return

    tasks[task]()


if __name__ == "__main__":
    main()
