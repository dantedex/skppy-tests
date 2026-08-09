// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_empty(const char* directory) {
    const char* fixture = "empty";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    size_t faces = 0;
    size_t materials = 0;

    opened = open_fixture(fixture, directory, "empty.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE_SDK(fixture, SUModelGetNumMaterials(model, &materials));
    REQUIRE(fixture, faces == 0, "expected no root faces");
    REQUIRE(fixture, materials == 0, "expected no materials");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("empty", validate_empty)
