#!/bin/bash

echo "Building for Linux..."
cmake -B build/linux -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux

echo "Building for Windows 64-bit (POSIX threads)..."
cmake -B build/win64 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/mingw64-toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build/win64
