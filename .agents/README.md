# Agent Guide

This repository is the public conformance suite for
<https://github.com/dantedex/skppy>.

- Author and maintainer: Dante Dex (`dante.dex.arch@gmail.com`)
- Repository: <https://github.com/dantedex/skppy-tests>

## Scope

`c/generators/` creates controlled SKP fixtures through the documented public
SketchUp C API. `python/validation/` inspects those files with `skppy`.
The inverse path pairs `python/writer/generators/test_<name>.py` with
`c/validators/writer/test_<name>.c`, so files written by Python are checked by
an independent public API implementation.

The suite contains original test code, redistributable assets, and the
explicitly approved SketchUp public headers and runtime/import libraries
tracked in `include/` and `bin/`.
Evidence must come from public documentation, documented public APIs,
controlled generated fixtures, or user-owned compatibility samples. Do not add
other SDK files, credentials, machine-specific paths, or assets with unclear
redistribution rights. Use only the public repository URLs listed above.

## Layout

- `c/generators/`: C-to-Python fixture producers.
- `python/validation/`: semantic parser checks.
- `python/writer/generators/`: Python-to-C fixture producers.
- `c/validators/writer/`: independent writer validators.
- `c/validators/blender/`: Blender-export validator.
- `python/version_matrix.py`: cross-version orchestration and report.

## Validation

```bash
python -m pytest -q
python run_tests.py
make writer-validation SKPPY_PATH=/path/to/skppy
make legacy-writer-validation SKPPY_PATH=/path/to/skppy
make release-matrix SKPPY_PATH=/path/to/skppy
```

Keep each writer fixture in its own generator and validator file. Parser tests
must assert target-version capabilities rather than filling missing historical
fields with modern defaults.
