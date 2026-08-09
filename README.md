# skppy-tests

Independent conformance tests for the
[`skppy`](https://github.com/dantedex/skppy) SKP reader and writer.

- **Author and maintainer:** Dante Dex
  ([dante.dex.arch@gmail.com](mailto:dante.dex.arch@gmail.com))
- **Repository:**
  [dantedex/skppy-tests](https://github.com/dantedex/skppy-tests)
- **Library:** [dantedex/skppy](https://github.com/dantedex/skppy)

The suite tests both directions without using one `skppy` subsystem as the
oracle for another:

| Direction | Producer | Validator |
| --- | --- | --- |
| C → Python | Documented public SketchUp C API | `skppy` parser |
| Python → C | Public `skppy` builder/writer API | Documented public SketchUp C API |
| Blender → C | Packaged Blender addon | Documented public SketchUp C API |

The repository includes the public C API headers in `include/` and the four
runtime/import libraries in `bin/` needed by the Windows conformance
executables. Optional SDK documentation and fixture resources are not bundled.

## Requirements

- Python 3.10 or newer
- MinGW-w64 (`x86_64-w64-mingw32-gcc`)
- Wine on Linux for the SDK executables
- A local installation or checkout of `skppy`

The bundled SDK layout is:

```text
./
  include/SketchUpAPI/
  bin/
```

Set `SDK_DIR` to another SDK root to test against a different public SDK
release. Such a directory may also contain optional fixture resources.

## Repository layout

| Path | Purpose |
| --- | --- |
| `include/` | Bundled public SketchUp C API headers |
| `bin/` | Bundled Windows runtime and import libraries |
| `c/generators/` | Public-API programs that create parser fixtures |
| `python/validation/` | Version-aware semantic parser checks |
| `python/writer/generators/` | One public Python model generator per writer fixture |
| `c/validators/writer/` | One independent SDK validator per writer fixture |
| `c/validators/blender/` | Semantic validator for an addon export |
| `python/version_matrix.py` | Cross-version generation and report orchestration |

The current catalog contains 47 C-generated parser fixtures and 59 independent
Python-to-C writer fixture pairs.

## Fast Python checks

```bash
python -m pytest -q
python run_tests.py
```

`run_tests.py` reads fixtures from `data/` by default. Set `OUTPUT_DIR` to use
another generated fixture directory.

## Generate parser fixtures

Build and run all C generators with the SDK default save target:

```bash
make generate OUTPUT_DIR=/path/to/output
```

Select a specific public save target with `VERSION`:

```bash
make generate VERSION=SU8
make generate VERSION=SU2017
make generate VERSION=SU2021
```

Accepted values cover `SU3`–`SU8`, `SU2013`–`SU2021`, and `current`.

## Cross-version matrix

The matrix generates each supported save target into an isolated directory,
loads every result, and writes `matrix-report.json`:

```bash
make matrix \
  SKPPY_PATH=/path/to/skppy \
  OUTPUT_DIR=/path/to/output \
  MATRIX_JOBS=4
```

Use `make release-matrix` to add the version-aware semantic suite. Historical
fields are asserted according to the target's capability map; missing old
fields are not filled with modern defaults.

## Writer conformance

```bash
make writer-validation \
  SKPPY_PATH=/path/to/skppy \
  OUTPUT_DIR=/path/to/output

make legacy-writer-validation \
  SKPPY_PATH=/path/to/skppy \
  OUTPUT_DIR=/path/to/output
```

Each `python/writer/generators/test_<name>.py` file has a matching
`c/validators/writer/test_<name>.c`. The validator opens the produced SKP
through the public SDK and checks semantic values. This deliberately avoids a
writer-to-parser round trip. The legacy target forces the same fixture set to
SketchUp Make 2017 and applies version-aware SDK assertions.

## Blender export conformance

First ask the addon's headless integration test to retain its export, then pass
the containing directory to the independent validator:

```bash
make blender-validation \
  BLENDER_EXPORT_DIR=/path/to/blender-export \
  OUTPUT_DIR=/path/to/output
```

Separate validators cover the modern export's nested/reused components,
transforms, PBR material state, embedded texture data, physical texture scale,
normalized UVs, tags, text, custom properties, and camera scenes, plus the
SketchUp Make 2017 export's exact model version, flattened geometry, coplanar
face merging, and inner-loop holes.

## Configuration

| Variable | Default | Purpose |
| --- | --- | --- |
| `SDK_DIR` | `.` | SketchUp C API SDK root containing `include/` and `bin/` |
| `SKPPY_PATH` | `../skppy` | Library checkout used by Python generators |
| `OUTPUT_DIR` | repository build directories | Generated binaries, fixtures, and reports |
| `VERSION` | SDK current | Save target used by `make generate` |
| `VERSIONS` | all supported targets | Targets used by the matrix |
| `MATRIX_JOBS` | `1` | Parallel generator processes |
| `BLENDER_EXPORT_DIR` | integration output directory | Location of `blender_export.skp` and `blender_export_2017.skp` |

## Contribution rules

- Keep every writer fixture in its own Python generator and C validator.
- Use public API calls and redistributable test assets only.
- Keep target capabilities centralized in `python/validation/common.py` rather
  than scattering version checks.
- Do not commit generated SKP files, unapproved SDK files, build products,
  credentials, or machine-specific paths.
- Update this README when the fixture catalog or invocation changes.

## License

The original test-suite source is available under the MIT License. See
[LICENSE](LICENSE). The third-party SketchUp headers in `include/` and
runtime/import libraries in `bin/` remain subject to their vendor's license and
are not covered by the MIT license.
