#!/bin/bash

BUILD_DIR="build"
SOURCE_DIR="source"
EXECUTABLE_NAME="http_server_core"

if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory: $BUILD_DIR"
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

cmake ../"$SOURCE_DIR"

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed."
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "ERROR: Make compilation failed."
    exit 1
fi

if [ ! -f "$EXECUTABLE_NAME" ]; then
    echo "ERROR: Executable '$EXECUTABLE_NAME' not found in $BUILD_DIR."
    cd ..
    exit 1
fi

./"$EXECUTABLE_NAME"

cd ..
