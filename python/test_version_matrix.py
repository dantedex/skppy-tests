# SPDX-License-Identifier: MIT
"""Tests for the SDK format matrix runner."""

import os
import sys
from pathlib import Path
from types import SimpleNamespace

import pytest

from version_matrix import (
    GeneratorResult,
    SDK_FORMAT_VERSIONS,
    changed_skp_files,
    import_fixture,
    normalize_versions,
    retry_failed_generators,
    run_semantic_validation,
)


def test_sdk_versions_cover_every_unique_enum_format() -> None:
    """Keep Current out because it aliases the versionless SU2021 format."""
    assert SDK_FORMAT_VERSIONS == (
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


def test_normalize_versions_is_case_insensitive_and_deduplicates() -> None:
    """Normalize matrix CLI input without running the same format twice."""
    assert normalize_versions(["su8", "SU2013", "SU8"]) == ("SU8", "SU2013")


def test_normalize_versions_rejects_unknown_format() -> None:
    """Do not silently fall back to the SDK current format on a typo."""
    with pytest.raises(ValueError, match="SU2022"):
        normalize_versions(["SU2022"])


def test_changed_skp_files_detects_new_and_rewritten_paths() -> None:
    """A generator counts only when it actually writes its fixture."""
    unchanged = Path("data/unchanged.skp")
    rewritten = Path("data/rewritten.skp")
    created = Path("data/created.skp")
    before = {unchanged: (10, 1), rewritten: (20, 2)}
    after = {unchanged: (10, 1), rewritten: (21, 3), created: (5, 4)}

    assert changed_skp_files(before, after) == [created, rewritten]


def test_retry_failed_generators_retries_only_failures_serially() -> None:
    """Recover transient failures without rerunning successful generators."""
    executables = [Path("ok.exe"), Path("transient.exe"), Path("broken.exe")]
    initial = [
        GeneratorResult("ok.exe", 0, 0.1, ["ok.skp"]),
        GeneratorResult("transient.exe", 0, 0.1, [], error="no output"),
        GeneratorResult("broken.exe", 1, 0.1, [], error="failed"),
    ]
    calls: list[str] = []

    def retry(executable: Path) -> GeneratorResult:
        calls.append(executable.name)
        if executable.name == "transient.exe":
            return GeneratorResult(executable.name, 0, 0.1, ["fixed.skp"])
        return GeneratorResult(executable.name, 1, 0.1, [], error="still failed")

    results = retry_failed_generators(initial, executables, retry, retries=1)

    assert calls == ["transient.exe", "broken.exe"]
    assert [result.attempts for result in results] == [1, 2, 2]
    assert results[1].error is None
    assert results[2].error == "still failed"


def test_semantic_validation_uses_requested_fixture_and_skppy_paths(
    monkeypatch: pytest.MonkeyPatch, tmp_path: Path
) -> None:
    """Pass one target and the tested checkout to the semantic subprocess."""
    runner = tmp_path / "runner.py"
    runner.write_text("# test runner placeholder\n", encoding="utf-8")
    captured = {}

    def run(command, **kwargs):
        captured["command"] = command
        captured["environment"] = kwargs["env"]
        return SimpleNamespace(returncode=0, stdout="Ran 47 tests\nOK", stderr="")

    monkeypatch.setattr("version_matrix.subprocess.run", run)
    result = run_semantic_validation(
        tmp_path / "SU8",
        runner=runner,
        skppy_path=tmp_path / "skppy",
        timeout=30.0,
    )

    assert result.status == "passed"
    assert captured["command"] == [sys.executable, str(runner.resolve())]
    assert captured["environment"]["OUTPUT_DIR"] == str((tmp_path / "SU8").resolve())
    assert captured["environment"]["PYTHONPATH"].split(os.pathsep)[0] == str(
        (tmp_path / "skppy").resolve()
    )


@pytest.mark.parametrize(
    ("legacy_archive", "expected_parser"),
    [(object(), "legacy"), (None, "modern")],
)
def test_import_fixture_reports_the_active_parser(
    tmp_path: Path,
    legacy_archive: object | None,
    expected_parser: str,
) -> None:
    """Use the public Model field when classifying matrix import results."""
    entities = SimpleNamespace(vertices=[], edges=[], faces=[])
    model = SimpleNamespace(
        entities=entities,
        definitions=[],
        materials=[],
        layers=[],
        scenes=[],
        header=SimpleNamespace(version_string="fixture"),
        legacy_archive=legacy_archive,
    )
    skppy_module = SimpleNamespace(load=lambda _path: model)

    result = import_fixture(tmp_path / "fixture.skp", skppy_module)

    assert result.status == "loaded"
    assert result.parser == expected_parser
