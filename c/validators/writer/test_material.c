// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_material(const char* directory) {
    const char* fixture = "material";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUMaterialRef material = SU_INVALID;
    SUMaterialRef face_material = SU_INVALID;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SUColor color = {0};
    bool use_opacity = false;
    double opacity = 0.0;
    double metallic = 0.0;
    double roughness = 0.0;
    int major = 0;
    int minor = 0;
    int build = 0;
    char name[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "material.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE(fixture, find_material(model, "Writer PBR", &material),
            "could not find Writer PBR");
    REQUIRE_SDK(fixture, SUMaterialGetColor(material, &color));
    REQUIRE_SDK(fixture, SUMaterialGetUseOpacity(material, &use_opacity));
    REQUIRE_SDK(fixture, SUMaterialGetOpacity(material, &opacity));
    REQUIRE_SDK(fixture, SUMaterialGetMetallicFactor(material, &metallic));
    REQUIRE_SDK(fixture, SUMaterialGetRoughnessFactor(material, &roughness));
    REQUIRE(fixture, color.red == 12 && color.green == 34 && color.blue == 56,
            "base color does not match the Python model");
    REQUIRE(fixture, use_opacity, "material opacity is not enabled");
    REQUIRE(fixture, almost_equal(opacity, 0.4), "material opacity is not 0.4");
    if (major > 17) {
        REQUIRE(fixture, almost_equal(metallic, 0.75), "metallic factor is not 0.75");
        REQUIRE(fixture, almost_equal(roughness, 0.125), "roughness factor is not 0.125");
    }

    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve material face");
    REQUIRE_SDK(fixture, SUFaceGetFrontMaterial(face, &face_material));
    REQUIRE(fixture, material_name(face_material, name), "could not read face material name");
    REQUIRE(fixture, strcmp(name, "Writer PBR") == 0,
            "face does not reference Writer PBR");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("material", validate_material)
