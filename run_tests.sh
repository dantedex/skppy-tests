#!/usr/bin/env bash

set -o pipefail

cd "$(dirname "$0")" || exit 1
# OUTPUT_DIR can be set externally to redirect where test fixtures are read.
# When unset, run_tests.py defaults to the project root.
exec python3 run_tests.py
