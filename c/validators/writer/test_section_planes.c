// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_section_planes(const char* directory) {
    const char* fixture = "section_planes";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUSectionPlaneRef section = SU_INVALID;
    SUStringRef text = SU_INVALID;
    struct SUPlane3D plane = {0};
    size_t count = 0;
    size_t retrieved = 0;
    int major = 0;
    int minor = 0;
    int build = 0;
    char name[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "section_planes.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumSectionPlanes(entities, &count));
    REQUIRE(fixture, count == 1, "expected one section plane");
    REQUIRE_SDK(fixture,
                SUEntitiesGetSectionPlanes(entities, 1, &section, &retrieved));
    REQUIRE_SDK(fixture, SUSectionPlaneGetPlane(section, &plane));
    REQUIRE(fixture,
            almost_equal(plane.a, 0) && almost_equal(plane.b, 0) &&
                almost_equal(plane.c, 1) && almost_equal(plane.d, -10),
            "section plane equation was not preserved");
    REQUIRE_SDK(fixture, SUStringCreate(&text));
    REQUIRE_SDK(fixture, SUSectionPlaneGetName(section, &text));
    REQUIRE(fixture, copy_utf8(text, name, sizeof(name)),
            "could not read section plane name");
    if (major > 17) {
        REQUIRE(fixture, strcmp(name, "TestSection") == 0,
                "section plane name was not preserved");
    }
    valid = true;
cleanup:
    if (text.ptr != NULL) SUStringRelease(&text);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("section_planes", validate_section_planes)
