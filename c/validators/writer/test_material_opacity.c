// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool check_opacity(
    SUModelRef model,
    const char* name,
    double expected,
    bool expected_enabled) {
    SUMaterialRef material = SU_INVALID;
    double opacity = -1.0;
    bool enabled = false;
    if (!find_material(model, name, &material)) return false;
    if (SUMaterialGetOpacity(material, &opacity) != SU_ERROR_NONE) return false;
    if (SUMaterialGetUseOpacity(material, &enabled) != SU_ERROR_NONE) return false;
    return almost_equal(opacity, expected) && enabled == expected_enabled;
}

static bool validate_material_opacity(const char* directory) {
    const char* fixture = "material_opacity";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    size_t faces = 0;

    opened = open_fixture(fixture, directory, "material_opacity.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture, check_opacity(model, "OpacityIgnored", 1.0, false),
            "opaque material unexpectedly enables opacity");
    REQUIRE(fixture, check_opacity(model, "Translucent", 0.35, true),
            "translucent opacity was not preserved");
    REQUIRE(fixture, check_opacity(model, "FullyTransparent", 0.0, true),
            "fully transparent opacity was not preserved");
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE(fixture, faces == 3, "expected one face per opacity material");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("material_opacity", validate_material_opacity)
