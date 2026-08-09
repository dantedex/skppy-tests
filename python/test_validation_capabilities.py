# SPDX-License-Identifier: MIT
"""Tests for save-format capability boundaries used by validation."""

from __future__ import annotations

import sys
from pathlib import Path
from types import SimpleNamespace

import pytest

VALIDATION_DIR = Path(__file__).parent / "validation"
sys.path.insert(0, str(VALIDATION_DIR))

from common import model_format_major, supports_feature  # noqa: E402


def _model(major: int | None, *, legacy: bool) -> SimpleNamespace:
    version = (major, 0, 0) if major is not None else None
    return SimpleNamespace(
        header=SimpleNamespace(version_tuple=version),
        legacy_archive=object() if legacy else None,
    )


@pytest.mark.parametrize("legacy", [True, False])
def test_capabilities_use_saved_version_for_every_container(legacy: bool) -> None:
    """Do not grant every feature merely because a file uses ZIP/TLV."""
    su2013 = _model(13, legacy=legacy)
    su2020 = _model(20, legacy=legacy)

    assert supports_feature(su2013, "attribute_dictionaries") is True
    assert supports_feature(su2013, "entity_attribute_dictionaries") is True
    assert supports_feature(su2013, "section_plane_name") is False
    assert supports_feature(su2013, "layer_folders") is False
    assert supports_feature(su2020, "layer_folders") is True
    assert supports_feature(su2020, "layer_folder_membership") is False


def test_legacy_capability_boundaries_match_observed_generations() -> None:
    """Keep old-format expectations at confirmed serialization boundaries."""
    su3 = _model(3, legacy=True)
    su4 = _model(4, legacy=True)
    su5 = _model(5, legacy=True)
    su7 = _model(7, legacy=True)

    assert supports_feature(su3, "component_behavior") is False
    assert supports_feature(su4, "component_behavior") is True
    assert supports_feature(su4, "entity_attribute_dictionaries") is True
    assert supports_feature(su4, "attribute_dictionaries") is False
    assert supports_feature(su4, "component_instance_name") is False
    assert supports_feature(su5, "component_instance_name") is True
    assert supports_feature(su7, "attribute_dictionaries") is True


def test_unknown_version_or_feature_is_not_silently_supported() -> None:
    """Fail closed for absent versions and loudly for capability typos."""
    model = _model(None, legacy=False)

    assert model_format_major(model) is None
    assert supports_feature(model, "component_behavior") is False
    with pytest.raises(KeyError):
        supports_feature(model, "not_a_feature")
