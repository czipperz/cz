#!/bin/bash

set -e

"$(dirname "$0")"/build-wrapper.sh build/release-bench Release -DCZ_BENCHMARK=1

./build/release-bench/bench
