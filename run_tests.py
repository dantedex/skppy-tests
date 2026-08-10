#!/usr/bin/env python3
"""Run all validation tests and report results.

skppy must be installed (pip install -e /path/to/skppy) before running.

Environment variables:
  OUTPUT_DIR  -- Root directory containing the generated test fixtures.
                 Defaults to the project root (parent of python/).
"""

import os
import sys
import unittest
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent
VALIDATION_DIR = PROJECT_ROOT / "python" / "validation"
sys.path.insert(0, str(VALIDATION_DIR))

if skppy_path := os.environ.get("SKPPY_PATH"):
    checkout = Path(skppy_path).resolve()
    if not (checkout / "skppy" / "__init__.py").is_file():
        raise RuntimeError(f"SKPPY_PATH is not a skppy checkout: {checkout}")
    sys.path.insert(0, str(checkout))

# Propagate OUTPUT_DIR to child tests via the environment.
# When set, validation scripts look for data under $OUTPUT_DIR/data/.
# When unset, they fall back to the project-root data/ directory.
if "OUTPUT_DIR" not in os.environ:
    os.environ["OUTPUT_DIR"] = str(PROJECT_ROOT)

loader = unittest.TestLoader()
suite = loader.discover(str(VALIDATION_DIR), pattern="test_val_*.py")
runner = unittest.TextTestRunner(verbosity=2)
result = runner.run(suite)
sys.exit(0 if result.wasSuccessful() else 1)
