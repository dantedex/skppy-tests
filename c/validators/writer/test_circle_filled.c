// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_circle_filled(const char* directory) {
    const char* fixture = "circle_filled";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SULoopRef outer = SU_INVALID;
    size_t faces = 0;
    size_t edges = 0;
    size_t vertices = 0;
    size_t holes = 0;

    opened = open_fixture(fixture, directory, "circle_filled.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &edges));
    REQUIRE(fixture, faces == 1 && edges == 24,
            "expected one 24-edge disk face");
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve disk face");
    REQUIRE_SDK(fixture, SUFaceGetOuterLoop(face, &outer));
    REQUIRE_SDK(fixture, SULoopGetNumVertices(outer, &vertices));
    REQUIRE_SDK(fixture, SUFaceGetNumInnerLoops(face, &holes));
    REQUIRE(fixture, vertices == 24 && holes == 0,
            "disk loop topology was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("circle_filled", validate_circle_filled)
