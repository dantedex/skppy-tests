// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_circle_hole(const char* directory) {
    const char* fixture = "circle_hole";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SULoopRef outer = SU_INVALID;
    SULoopRef inner = SU_INVALID;
    size_t faces = 0;
    size_t edges = 0;
    size_t outer_vertices = 0;
    size_t inner_vertices = 0;
    size_t holes = 0;
    size_t retrieved = 0;

    opened = open_fixture(fixture, directory, "circle_hole.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &edges));
    REQUIRE(fixture, faces == 1 && edges == 48,
            "expected one 48-edge annular face");
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve annular face");
    REQUIRE_SDK(fixture, SUFaceGetOuterLoop(face, &outer));
    REQUIRE_SDK(fixture, SULoopGetNumVertices(outer, &outer_vertices));
    REQUIRE_SDK(fixture, SUFaceGetNumInnerLoops(face, &holes));
    REQUIRE(fixture, holes == 1, "annular face does not have one hole");
    REQUIRE_SDK(fixture, SUFaceGetInnerLoops(face, 1, &inner, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve annular inner loop");
    REQUIRE_SDK(fixture, SULoopGetNumVertices(inner, &inner_vertices));
    REQUIRE(fixture, outer_vertices == 24 && inner_vertices == 24,
            "annular loops are not both 24 segments");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("circle_hole", validate_circle_hole)
