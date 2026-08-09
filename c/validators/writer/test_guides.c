// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_guides(const char* directory) {
    const char* fixture = "guides";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUGuidePointRef point = SU_INVALID;
    SUGuideLineRef line = SU_INVALID;
    struct SUPoint3D position = {0};
    struct SUPoint3D origin = {0};
    struct SUVector3D direction = {0};
    bool infinite = true;
    size_t points = 0;
    size_t lines = 0;
    size_t retrieved = 0;

    opened = open_fixture(fixture, directory, "guides.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumGuidePoints(entities, &points));
    REQUIRE_SDK(fixture, SUEntitiesGetNumGuideLines(entities, &lines));
    REQUIRE(fixture, points == 1 && lines == 1,
            "expected one guide point and one guide line");
    REQUIRE_SDK(fixture, SUEntitiesGetGuidePoints(entities, 1, &point, &retrieved));
    REQUIRE_SDK(fixture, SUGuidePointGetPosition(point, &position));
    REQUIRE(fixture,
            almost_equal(position.x, 10) && almost_equal(position.y, 20) &&
                almost_equal(position.z, 30),
            "guide point position was not preserved");
    retrieved = 0;
    REQUIRE_SDK(fixture, SUEntitiesGetGuideLines(entities, 1, &line, &retrieved));
    REQUIRE_SDK(fixture, SUGuideLineGetData(line, &origin, &direction, &infinite));
    REQUIRE(fixture, !infinite, "guide line unexpectedly became infinite");
    REQUIRE(fixture,
            almost_equal(origin.x, 0) && almost_equal(origin.y, 0) &&
                almost_equal(origin.z, 0) && almost_equal(direction.x, 100) &&
                almost_equal(direction.y, 100) && almost_equal(direction.z, 100),
            "finite guide line endpoints were not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("guides", validate_guides)
