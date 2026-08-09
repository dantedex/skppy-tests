// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_pyramid(const char* directory) {
    const char* fixture = "pyramid";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "pyramid.skp", &model);
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    size_t edges = 0;
    size_t faces = 0;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &edges));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE(fixture, edges == 8 && faces == 5, "pyramid topology is not 8 edges / 5 faces");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("pyramid", validate_pyramid)
