#!/bin/bash

set -e

cd "$(dirname "$0")"
mkdir -p build/debug

(cd build/debug
 cmake -DCMAKE_BUILD_TYPE=Debug -DCZ_BUILD_TESTS=1 ../.. >/dev/null
 cmake --build .)

./run-tests.sh
