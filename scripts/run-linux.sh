#!/usr/bin/env bash
set -e
cmake -S . -B build -G Ninja -DFORGE_BUILD_TESTS=ON
cmake --build build -j2
ctest --test-dir build --output-on-failure
exec ./build/ForgeEngine
