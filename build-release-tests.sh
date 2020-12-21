#!/bin/bash

set -e

"$(dirname "$0")"/build-wrapper.sh build/release-tests Release -DCZ_BUILD_TESTS=1

./build/release-tests/test --use-colour=no
