#!/bin/sh
set -e

mkdir -p build
cmake -S . -B build
cmake --build build --target main
./build/main