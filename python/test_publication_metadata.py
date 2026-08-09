# SPDX-License-Identifier: MIT
"""Checks for the public identity distributed with skppy-tests."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def test_license_and_readme_identify_public_project() -> None:
    license_text = (ROOT / "LICENSE").read_text(encoding="utf-8")
    readme = (ROOT / "README.md").read_text(encoding="utf-8")

    assert "Copyright (c) 2026 Dante Dex" in license_text
    assert "dante.dex.arch@gmail.com" in readme
    assert "https://github.com/dantedex/skppy-tests" in readme
    assert "https://github.com/dantedex/skppy" in readme
