// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_extreme_coordinates(const char* directory) {
    const char* fixture = "extreme_coordinates";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "extreme_coordinates.skp", &model);
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUEdgeRef edge = SU_INVALID;
    SUVertexRef vertex = SU_INVALID;
    struct SUPoint3D position = {0};
    size_t retrieved = 0;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetEdges(entities, false, 1, &edge, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve extreme edge");
    REQUIRE_SDK(fixture, SUEdgeGetStartVertex(edge, &vertex));
    REQUIRE_SDK(fixture, SUVertexGetPosition(vertex, &position));
    REQUIRE(fixture,
            almost_equal(position.x, 1.0e12 + 0.125) &&
                almost_equal(position.y, -1.0e12 - 0.25) &&
                almost_equal(position.z, 0.375),
            "extreme coordinate precision was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("extreme_coordinates", validate_extreme_coordinates)
