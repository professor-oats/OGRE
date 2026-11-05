#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e

# Get the absolute path of the directory where this script lives (Project Directory)
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/debug_build"

# Check if the build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory '$BUILD_DIR' not found!"
    echo "Please make sure your 'debug_build' folder exists."
    exit 1
fi

echo "=========================================="
echo " Building MySecondApp (Debug Mode)"
echo "=========================================="

# Navigate to the build directory and run the build command
cd "$BUILD_DIR"
cmake --build . --config Debug

echo "=========================================="
echo " Build finished successfully! 🎉"
echo "=========================================="
