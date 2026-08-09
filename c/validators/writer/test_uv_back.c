// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_uv_back(const char* directory) {
    const char* fixture = "uv_back";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SUMaterialRef material = SU_INVALID;
    SUMaterialRef back_material = SU_INVALID;
    char name[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "uv_back.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve two-sided face");
    REQUIRE_SDK(fixture, SUFaceGetFrontMaterial(face, &material));
    REQUIRE(fixture, material_name(material, name),
            "could not read front material name");
    REQUIRE(fixture, strcmp(name, "Writer Front") == 0,
            "front face material was not preserved");
    REQUIRE_SDK(fixture, SUFaceGetBackMaterial(face, &back_material));
    REQUIRE(fixture, material_name(back_material, name),
            "could not read back material name");
    REQUIRE(fixture, strcmp(name, "Writer Back") == 0,
            "back face material was not preserved");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("uv_back", validate_uv_back)
