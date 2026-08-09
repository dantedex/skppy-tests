// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_uv_back_mat(const char* directory) {
    const char* fixture = "uv_back_mat";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SUMaterialRef material = SU_INVALID;
    SUUVHelperRef helper = SU_INVALID;
    SUTextureWriterRef texture_writer = SU_INVALID;
    struct SUPoint3D points[2] = {{0, 0, 0}, {100, 0, 0}};
    struct SUUVQ front[2] = {{0}};
    struct SUUVQ back[2] = {{0}};
    char name[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "uv_back_mat.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve UV face");
    REQUIRE_SDK(fixture, SUFaceGetFrontMaterial(face, &material));
    REQUIRE(fixture, material_name(material, name),
            "could not read front material name");
    REQUIRE(fixture, strcmp(name, "FrontMaterial") == 0,
            "front material was not preserved");
    REQUIRE_SDK(fixture, SUFaceGetBackMaterial(face, &material));
    REQUIRE(fixture, material_name(material, name),
            "could not read back material name");
    REQUIRE(fixture, strcmp(name, "BackMaterial") == 0,
            "back material was not preserved");
    REQUIRE_SDK(
        fixture, SUFaceGetUVHelper(face, true, true, texture_writer, &helper));
    for (size_t index = 0; index < 2; ++index) {
        REQUIRE_SDK(fixture, SUUVHelperGetFrontUVQ(helper, &points[index], &front[index]));
        REQUIRE_SDK(fixture, SUUVHelperGetBackUVQ(helper, &points[index], &back[index]));
    }
    REQUIRE(fixture,
            almost_equal(front[0].u / front[0].q, 0.0) &&
                almost_equal(front[0].v / front[0].q, 0.0) &&
                almost_equal(front[1].u / front[1].q, 1.0) &&
                almost_equal(front[1].v / front[1].q, 0.0),
            "front UV mapping was not preserved");
    REQUIRE(fixture,
            almost_equal(back[0].u / back[0].q, 0.0) &&
                almost_equal(back[0].v / back[0].q, 1.0) &&
                almost_equal(back[1].u / back[1].q, 0.0) &&
                almost_equal(back[1].v / back[1].q, 0.0),
            "rotated back UV mapping was not preserved");
    valid = true;

cleanup:
    if (helper.ptr != NULL) SUUVHelperRelease(&helper);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("uv_back_mat", validate_uv_back_mat)
