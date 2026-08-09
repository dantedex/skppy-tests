#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Basic smoke-test: try to load every .skp file in data/ with skppy.

Environment variables:
  OUTPUT_DIR  - Root directory containing the generated test fixtures.
                Defaults to the project root.
"""

import os
import sys

import skppy

# Resolve data directory from OUTPUT_DIR or fall back to project root.
_OUTPUT_ROOT = os.environ.get("OUTPUT_DIR")
if _OUTPUT_ROOT:
    DATA_DIR = os.path.join(_OUTPUT_ROOT, "data")
else:
    _project_root = os.path.dirname(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    DATA_DIR = os.path.join(_project_root, "data")

passed = 0
failed = 0
skipped = 0

for fname in sorted(os.listdir(DATA_DIR)):
    if not fname.endswith(".skp"):
        continue
    path = os.path.join(DATA_DIR, fname)
    try:
        model = skppy.load(path)
        print(f"  OK   {fname}")
        passed += 1
    except FileNotFoundError:
        print(f"  SKIP {fname} (not found)")
        skipped += 1
    except Exception as e:
        print(f"  FAIL {fname}: {e}")
        failed += 1

print(f"\nResults: {passed} passed, {failed} failed, {skipped} skipped")
if failed:
    sys.exit(1)
