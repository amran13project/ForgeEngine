#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/build-qa"
rm -rf "$BUILD"
cmake -S "$ROOT" -B "$BUILD" -DFORGE_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD" -j2
ctest --test-dir "$BUILD" --output-on-failure
"$ROOT/scripts/native-smoke-test.sh"
printf '\n[QA] Native build + all CTest suites + smoke tests: PASS\n'
