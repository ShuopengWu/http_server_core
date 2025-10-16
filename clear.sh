#!/bin/bash

BUILD_DIR="build"

if ! rm -rf "$BUILD_DIR"; then
    echo "ERROR: Failed to remove $BUILD_DIR. Please check permissions."
    exit 1
fi

echo "Clear build directory success."