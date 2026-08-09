# SPDX-License-Identifier: MIT
"""Structural checks for the one-generator/one-validator inverse catalog."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def test_every_writer_generator_has_one_c_validator() -> None:
    """Keep fixture ownership isolated and paired by exact basename."""
    generators = {
        path.stem for path in (ROOT / "python/writer/generators").glob("test_*.py")
    }
    validators = {
        path.stem
        for path in (ROOT / "c/validators/writer").glob("test_*.c")
        if path.name != "test_utils.c"
    }

    assert generators == validators
    assert len(generators) == 62


def test_writer_fixture_semantics_are_not_monolithic() -> None:
    """Prevent the deleted dispatch files from becoming parallel authorities."""
    assert not (ROOT / "python/writer/generate_fixtures.py").exists()
    assert not (ROOT / "c/validators/validate_writer.c").exists()
