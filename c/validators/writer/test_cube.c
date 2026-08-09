// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_cube(const char* directory) {
    const char* fixture = "cube";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "cube.skp", &model);
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    size_t edges = 0;
    size_t faces = 0;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &edges));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE(fixture, edges == 12 && faces == 6, "cube topology is not 12 edges / 6 faces");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("cube", validate_cube)
