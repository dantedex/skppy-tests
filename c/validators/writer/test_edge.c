// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_edge(const char* directory) {
    const char* fixture = "edge";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "edge.skp", &model);
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUEdgeRef edge = SU_INVALID;
    SUVertexRef vertex = SU_INVALID;
    struct SUPoint3D position = {0};
    size_t count = 0;
    size_t retrieved = 0;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &count));
    REQUIRE(fixture, count == 1, "expected one edge");
    REQUIRE_SDK(fixture, SUEntitiesGetEdges(entities, false, 1, &edge, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve edge");
    REQUIRE_SDK(fixture, SUEdgeGetStartVertex(edge, &vertex));
    REQUIRE_SDK(fixture, SUVertexGetPosition(vertex, &position));
    REQUIRE(fixture,
            almost_equal(position.x, 1.25) && almost_equal(position.y, -2.5) &&
                almost_equal(position.z, 3.75),
            "edge start position was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("edge", validate_edge)
