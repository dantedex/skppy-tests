// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_construction(const char* directory) {
    const char* fixture = "construction";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUGuideLineRef line = SU_INVALID;
    SUGuidePointRef point = SU_INVALID;
    SUSectionPlaneRef section = SU_INVALID;
    SULayerRef layer = SU_INVALID;
    SUStringRef text = SU_INVALID;
    struct SUPoint3D line_point = {0};
    struct SUVector3D direction = {0};
    struct SUPoint3D position = {0};
    struct SUPlane3D plane = {0};
    bool infinite = false;
    size_t lines = 0;
    size_t points = 0;
    size_t sections = 0;
    size_t retrieved = 0;
    int major = 0;
    int minor = 0;
    int build = 0;
    char name[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "construction.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumGuideLines(entities, &lines));
    REQUIRE_SDK(fixture, SUEntitiesGetNumGuidePoints(entities, &points));
    REQUIRE_SDK(fixture, SUEntitiesGetNumSectionPlanes(entities, &sections));
    REQUIRE(fixture, lines == 1 && points == 1 && sections == 1,
            "expected one guide line, guide point, and section plane");

    REQUIRE_SDK(fixture, SUEntitiesGetGuideLines(entities, 1, &line, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve guide line");
    REQUIRE_SDK(fixture, SUGuideLineGetData(line, &line_point, &direction, &infinite));
    REQUIRE(fixture, !infinite, "guide line is not finite");
    REQUIRE(fixture,
            almost_equal(line_point.x, 1.0) && almost_equal(line_point.y, 2.0) &&
                almost_equal(line_point.z, 3.0) && almost_equal(direction.x, 0.0) &&
                almost_equal(direction.y, 5.0) && almost_equal(direction.z, 0.0),
            "guide line geometry was not preserved");
    REQUIRE_SDK(
        fixture, SUDrawingElementGetLayer(SUGuideLineToDrawingElement(line), &layer));
    REQUIRE(fixture, layer_name(layer, name), "could not read guide line layer");
    REQUIRE(fixture, strcmp(name, "Writer Construction") == 0,
            "guide line layer was not preserved");

    retrieved = 0;
    REQUIRE_SDK(fixture, SUEntitiesGetGuidePoints(entities, 1, &point, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve guide point");
    REQUIRE_SDK(fixture, SUGuidePointGetPosition(point, &position));
    REQUIRE(fixture,
            almost_equal(position.x, 4.0) && almost_equal(position.y, 5.0) &&
                almost_equal(position.z, 6.0),
            "guide point position was not preserved");
    REQUIRE_SDK(
        fixture, SUDrawingElementGetLayer(SUGuidePointToDrawingElement(point), &layer));
    REQUIRE(fixture, layer_name(layer, name), "could not read guide point layer");
    REQUIRE(fixture, strcmp(name, "Writer Construction") == 0,
            "guide point layer was not preserved");

    retrieved = 0;
    REQUIRE_SDK(
        fixture, SUEntitiesGetSectionPlanes(entities, 1, &section, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve section plane");
    REQUIRE_SDK(fixture, SUSectionPlaneGetPlane(section, &plane));
    REQUIRE(fixture,
            almost_equal(plane.a, 1.0) && almost_equal(plane.b, 0.0) &&
                almost_equal(plane.c, 0.0) && almost_equal(plane.d, -7.0),
            "section plane equation was not preserved");
    REQUIRE_SDK(fixture, SUStringCreate(&text));
    REQUIRE_SDK(fixture, SUSectionPlaneGetName(section, &text));
    REQUIRE(fixture, copy_utf8(text, name, NAME_CAPACITY),
            "could not read section plane name");
    if (major > 17) {
        REQUIRE(fixture, strcmp(name, "Writer Section") == 0,
                "section plane name was not preserved");
    }
    REQUIRE_SDK(
        fixture, SUDrawingElementGetLayer(SUSectionPlaneToDrawingElement(section), &layer));
    REQUIRE(fixture, layer_name(layer, name), "could not read section plane layer");
    REQUIRE(fixture, strcmp(name, "Writer Construction") == 0,
            "section plane layer was not preserved");
    valid = true;

cleanup:
    if (text.ptr != NULL) SUStringRelease(&text);
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("construction", validate_construction)
