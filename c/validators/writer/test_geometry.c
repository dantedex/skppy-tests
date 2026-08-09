// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_geometry(const char* directory) {
    const char* fixture = "geometry";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    size_t faces = 0;
    size_t edges = 0;
    size_t vertices = 0;

    opened = open_fixture(fixture, directory, "geometry.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &edges));
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve the triangle face");
    REQUIRE_SDK(fixture, SUFaceGetNumVertices(face, &vertices));
    REQUIRE(fixture, faces == 1, "expected one root face");
    REQUIRE(fixture, edges == 3, "expected three face edges");
    REQUIRE(fixture, vertices == 3, "expected three face vertices");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("geometry", validate_geometry)
