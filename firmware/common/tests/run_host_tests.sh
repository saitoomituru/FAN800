#!/bin/sh
set -eu

root_dir=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
output="${TMPDIR:-/tmp}/fan800-safety-guard-test"

${CXX:-c++} -std=c++17 -Wall -Wextra -Werror \
  -I"$root_dir/firmware/common" \
  "$root_dir/firmware/common/safety_guard.cpp" \
  "$root_dir/firmware/common/tests/safety_guard_test.cpp" \
  -o "$output"

"$output"
