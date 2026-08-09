#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Generate and smoke-import SKP fixtures for every SDK save format."""

from __future__ import annotations

import argparse
import concurrent.futures
import importlib
import json
import os
import shutil
import subprocess
import sys
import time
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Callable, Iterable

SDK_FORMAT_VERSIONS = (
    "SU3",
    "SU4",
    "SU5",
    "SU6",
    "SU7",
    "SU8",
    "SU2013",
    "SU2014",
    "SU2015",
    "SU2016",
    "SU2017",
    "SU2018",
    "SU2019",
    "SU2020",
    "SU2021",
)


@dataclass(slots=True)
class GeneratorResult:
    """Result of running one C fixture generator."""

    generator: str
    returncode: int | None
    duration_seconds: float
    changed_files: list[str]
    error: str | None = None
    output_tail: str | None = None
    attempts: int = 1


@dataclass(slots=True)
class ImportResult:
    """Result of loading one generated SKP with skppy."""

    filename: str
    status: str
    duration_seconds: float
    source_version: str | None = None
    parser: str | None = None
    counts: dict[str, int] | None = None
    error_type: str | None = None
    error: str | None = None


@dataclass(slots=True)
class SemanticResult:
    """Result of the version-aware semantic suite for one save target."""

    status: str
    returncode: int | None
    duration_seconds: float
    error: str | None = None
    output_tail: str | None = None


def normalize_versions(values: Iterable[str]) -> tuple[str, ...]:
    """Normalize and validate requested SDK format names."""
    requested = tuple(value.upper() for value in values)
    unknown = sorted(set(requested).difference(SDK_FORMAT_VERSIONS))
    if unknown:
        raise ValueError(f"Unsupported SDK format version(s): {', '.join(unknown)}")
    return tuple(dict.fromkeys(requested))


def snapshot_skp_files(data_dir: Path) -> dict[Path, tuple[int, int]]:
    """Return size and modification time for every SKP below one data path."""
    if not data_dir.exists():
        return {}
    return {
        path: (path.stat().st_size, path.stat().st_mtime_ns)
        for path in data_dir.rglob("*.skp")
    }


def changed_skp_files(
    before: dict[Path, tuple[int, int]],
    after: dict[Path, tuple[int, int]],
) -> list[Path]:
    """Return files newly created or rewritten between two snapshots."""
    return sorted(path for path, state in after.items() if before.get(path) != state)


def _output_tail(completed: subprocess.CompletedProcess[str]) -> str | None:
    output = "\n".join(
        part.strip() for part in (completed.stdout, completed.stderr) if part.strip()
    )
    if not output:
        return None
    return "\n".join(output.splitlines()[-12:])


def run_generator(
    executable: Path,
    *,
    version: str,
    version_root: Path,
    wine: str,
    timeout: float,
    resources_dir: Path | None,
) -> GeneratorResult:
    """Run one generator and identify the fixture it actually wrote."""
    work_root = version_root / ".generator-work" / executable.stem
    if work_root.exists():
        shutil.rmtree(work_root)
    data_dir = work_root / "data"
    data_dir.mkdir(parents=True)
    if resources_dir is not None and resources_dir.is_dir():
        (work_root / "resources").symlink_to(
            resources_dir.resolve(), target_is_directory=True
        )
    before = snapshot_skp_files(data_dir)
    started = time.perf_counter()
    environment = os.environ.copy()
    environment.setdefault("WINEDEBUG", "-all")
    try:
        completed = subprocess.run(
            [wine, str(executable.resolve()), "-v", version],
            cwd=work_root,
            capture_output=True,
            check=False,
            env=environment,
            text=True,
            timeout=timeout,
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        return GeneratorResult(
            generator=executable.name,
            returncode=None,
            duration_seconds=round(time.perf_counter() - started, 6),
            changed_files=[],
            error=f"{type(exc).__name__}: {exc}",
        )

    changed = changed_skp_files(before, snapshot_skp_files(data_dir))
    destination = version_root / "data"
    destination.mkdir(parents=True, exist_ok=True)
    relative: list[str] = []
    for path in changed:
        target = destination / path.name
        shutil.copy2(path, target)
        relative.append(str(target.relative_to(version_root)))
    error = None
    if completed.returncode != 0:
        error = f"generator exited with status {completed.returncode}"
    elif not changed:
        error = "generator did not create or rewrite an SKP file"
    return GeneratorResult(
        generator=executable.name,
        returncode=completed.returncode,
        duration_seconds=round(time.perf_counter() - started, 6),
        changed_files=relative,
        error=error,
        output_tail=_output_tail(completed) if error else None,
    )


def retry_failed_generators(
    results: list[GeneratorResult],
    executables: list[Path],
    run: Callable[[Path], GeneratorResult],
    *,
    retries: int,
) -> list[GeneratorResult]:
    """Retry only failed generators serially and record total attempts."""
    final = list(results)
    for index, initial in enumerate(results):
        result = initial
        attempts = 1
        while result.error and attempts <= retries:
            result = run(executables[index])
            attempts += 1
        result.attempts = attempts
        final[index] = result
    return final


def _model_counts(model: Any) -> dict[str, int]:
    entities = model.entities
    return {
        "vertices": len(entities.vertices),
        "edges": len(entities.edges),
        "faces": len(entities.faces),
        "definitions": len(model.definitions),
        "materials": len(model.materials),
        "layers": len(model.layers),
        "scenes": len(model.scenes),
    }


def import_fixture(path: Path, skppy_module: Any) -> ImportResult:
    """Smoke-load one fixture and retain concise parser diagnostics."""
    started = time.perf_counter()
    try:
        model = skppy_module.load(path)
    except Exception as exc:  # The report must include every parser failure.
        return ImportResult(
            filename=path.name,
            status="failed",
            duration_seconds=round(time.perf_counter() - started, 6),
            error_type=type(exc).__name__,
            error=str(exc),
        )
    return ImportResult(
        filename=path.name,
        status="loaded",
        duration_seconds=round(time.perf_counter() - started, 6),
        source_version=model.header.version_string,
        parser="legacy" if model.legacy_archive is not None else "modern",
        counts=_model_counts(model),
    )


def run_semantic_validation(
    version_root: Path,
    *,
    runner: Path,
    skppy_path: Path,
    timeout: float,
) -> SemanticResult:
    """Run the semantic suite against one generated version directory."""
    environment = os.environ.copy()
    environment["OUTPUT_DIR"] = str(version_root.resolve())
    python_path = str(skppy_path.resolve())
    if inherited := environment.get("PYTHONPATH"):
        python_path = python_path + os.pathsep + inherited
    environment["PYTHONPATH"] = python_path
    started = time.perf_counter()
    try:
        completed = subprocess.run(
            [sys.executable, str(runner.resolve())],
            capture_output=True,
            check=False,
            env=environment,
            text=True,
            timeout=timeout,
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        return SemanticResult(
            status="failed",
            returncode=None,
            duration_seconds=round(time.perf_counter() - started, 6),
            error=f"{type(exc).__name__}: {exc}",
        )

    output_tail = _output_tail(completed)
    if completed.returncode:
        return SemanticResult(
            status="failed",
            returncode=completed.returncode,
            duration_seconds=round(time.perf_counter() - started, 6),
            error=f"semantic suite exited with status {completed.returncode}",
            output_tail=output_tail,
        )
    return SemanticResult(
        status="passed",
        returncode=0,
        duration_seconds=round(time.perf_counter() - started, 6),
        output_tail=output_tail,
    )


def _load_skppy(skppy_path: Path) -> Any:
    resolved = str(skppy_path.resolve())
    if resolved not in sys.path:
        sys.path.insert(0, resolved)
    return importlib.import_module("skppy")


def run_matrix(args: argparse.Namespace) -> dict[str, Any]:
    """Generate and import the requested version matrix."""
    versions = normalize_versions(args.versions)
    executables = sorted(args.bin_dir.glob("test_*.exe"))
    if not args.skip_generation and not executables:
        raise RuntimeError(f"No test_*.exe generators found in {args.bin_dir}")

    skppy_module = _load_skppy(args.skppy_path)
    report: dict[str, Any] = {
        "sdk_format_versions": list(versions),
        "generator_count": len(executables),
        "generation_skipped": args.skip_generation,
        "skppy_version": getattr(skppy_module, "__version__", "unknown"),
        "versions": {},
    }

    total_generation_failures = 0
    total_import_failures = 0
    total_semantic_failures = 0
    total_files = 0
    for version in versions:
        version_root = args.output_dir / version
        if args.clean and not args.skip_generation and version_root.exists():
            shutil.rmtree(version_root)
        (version_root / "data").mkdir(parents=True, exist_ok=True)
        generation: list[GeneratorResult] = []
        if args.skip_generation:
            print(f"\n[{version}] reusing existing fixtures", flush=True)
        else:
            print(f"\n[{version}] generating {len(executables)} fixtures", flush=True)

            def generate_one(executable: Path) -> GeneratorResult:
                return run_generator(
                    executable,
                    version=version,
                    version_root=version_root,
                    wine=args.wine,
                    timeout=args.generator_timeout,
                    resources_dir=args.resources_dir,
                )

            with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
                futures = [
                    pool.submit(generate_one, executable) for executable in executables
                ]
                generation = [future.result() for future in futures]
            generation = retry_failed_generators(
                generation,
                executables,
                generate_one,
                retries=args.generator_retries,
            )

        generation_failures = [item for item in generation if item.error]
        fixtures = sorted((version_root / "data").glob("*.skp"))
        if args.skip_generation:
            print(f"[{version}] found {len(fixtures)}; smoke-importing", flush=True)
        else:
            successful_generators = len(generation) - len(generation_failures)
            print(
                f"[{version}] generated {len(fixtures)} files from "
                f"{successful_generators}/{len(executables)} generators; "
                "smoke-importing",
                flush=True,
            )
        imports = [import_fixture(path, skppy_module) for path in fixtures]
        import_failures = [item for item in imports if item.status != "loaded"]
        print(
            f"[{version}] loaded {len(imports) - len(import_failures)}/{len(imports)}; "
            f"generator gaps {len(generation_failures)}",
            flush=True,
        )
        semantic = None
        if args.semantic_validation:
            semantic = run_semantic_validation(
                version_root,
                runner=args.semantic_runner,
                skppy_path=args.skppy_path,
                timeout=args.semantic_timeout,
            )
            print(f"[{version}] semantic validation {semantic.status}", flush=True)

        report["versions"][version] = {
            "generated_file_count": len(fixtures),
            "generation_failure_count": len(generation_failures),
            "import_failure_count": len(import_failures),
            "generation": [asdict(item) for item in generation],
            "imports": [asdict(item) for item in imports],
            "semantic": asdict(semantic) if semantic is not None else None,
        }
        total_generation_failures += len(generation_failures)
        total_import_failures += len(import_failures)
        total_semantic_failures += int(
            semantic is not None and semantic.status != "passed"
        )
        total_files += len(fixtures)

    report["summary"] = {
        "version_count": len(versions),
        "generated_file_count": total_files,
        "generation_failure_count": total_generation_failures,
        "import_failure_count": total_import_failures,
        "loaded_file_count": total_files - total_import_failures,
        "semantic_failure_count": total_semantic_failures,
        "semantic_validation_enabled": args.semantic_validation,
    }
    return report


def print_failure_summary(report: dict[str, Any]) -> None:
    """Print matrix failures and captured subprocess output to the terminal."""
    failures: list[tuple[str, str, str | None]] = []
    for version, version_report in report["versions"].items():
        for result in version_report["generation"]:
            if error := result["error"]:
                failures.append(
                    (
                        f"[{version}] generator {result['generator']}",
                        error,
                        result["output_tail"],
                    )
                )
        for result in version_report["imports"]:
            if result["status"] != "loaded":
                error = result["error"] or "fixture could not be loaded"
                failures.append(
                    (
                        f"[{version}] import {result['filename']}",
                        f"{result['error_type'] or 'Error'}: {error}",
                        None,
                    )
                )
        semantic = version_report["semantic"]
        if semantic is not None and semantic["status"] != "passed":
            failures.append(
                (
                    f"[{version}] semantic validation",
                    semantic["error"] or "semantic validation failed",
                    semantic["output_tail"],
                )
            )

    if not failures:
        return
    print("\nMatrix failures:", file=sys.stderr, flush=True)
    for subject, error, output_tail in failures:
        print(f"  {subject}: {error}", file=sys.stderr, flush=True)
        if output_tail:
            for line in output_tail.splitlines():
                print(f"    {line}", file=sys.stderr, flush=True)


def build_parser() -> argparse.ArgumentParser:
    """Build the command-line parser."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("versions", nargs="*", default=SDK_FORMAT_VERSIONS)
    parser.add_argument("--bin-dir", type=Path, default=Path("bin"))
    parser.add_argument("--output-dir", type=Path, default=Path("matrix"))
    parser.add_argument("--skppy-path", type=Path, default=Path("../skppy"))
    parser.add_argument("--wine", default="wine")
    parser.add_argument("--resources-dir", type=Path)
    parser.add_argument("--jobs", type=int, default=1)
    parser.add_argument("--generator-timeout", type=float, default=120.0)
    parser.add_argument("--generator-retries", type=int, default=1)
    parser.add_argument("--semantic-validation", action="store_true")
    parser.add_argument("--semantic-runner", type=Path, default=Path("run_tests.py"))
    parser.add_argument("--semantic-timeout", type=float, default=120.0)
    parser.add_argument("--clean", action="store_true")
    parser.add_argument("--skip-generation", action="store_true")
    parser.add_argument("--report", type=Path)
    return parser


def main() -> int:
    """Run the CLI and return a process status."""
    parser = build_parser()
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error("--jobs must be at least 1")
    if args.generator_retries < 0:
        parser.error("--generator-retries cannot be negative")
    if args.semantic_validation and not args.semantic_runner.is_file():
        parser.error(f"semantic runner not found: {args.semantic_runner}")
    try:
        report = run_matrix(args)
    except (RuntimeError, ValueError) as exc:
        parser.error(str(exc))

    report_path = args.report or args.output_dir / "matrix-report.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(
        json.dumps(report, indent=2, sort_keys=True), encoding="utf-8"
    )
    summary = report["summary"]
    print(f"\nReport: {report_path}")
    print(json.dumps(summary, indent=2, sort_keys=True))
    print_failure_summary(report)
    return (
        1
        if (
            summary["generation_failure_count"]
            or summary["import_failure_count"]
            or summary["semantic_failure_count"]
        )
        else 0
    )


if __name__ == "__main__":
    raise SystemExit(main())
