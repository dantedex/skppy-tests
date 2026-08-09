// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_textures(const char* directory) {
    const char* fixture = "textures";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUMaterialRef material = SU_INVALID;
    SUMaterialRef face_material = SU_INVALID;
    SUTextureRef texture = SU_INVALID;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    size_t width = 0;
    size_t height = 0;
    double s_scale = 0.0;
    double t_scale = 0.0;
    char name[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "textures.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture, find_material(model, "TexturedMaterial", &material),
            "could not find TexturedMaterial");
    REQUIRE_SDK(fixture, SUMaterialGetTexture(material, &texture));
    REQUIRE_SDK(
        fixture, SUTextureGetDimensions(texture, &width, &height, &s_scale, &t_scale));
    REQUIRE(fixture, width == 1 && height == 1, "embedded texture is not 1x1");
    REQUIRE(fixture, almost_equal(s_scale, 0.01) && almost_equal(t_scale, 0.01),
            "texture physical scale is not 100 by 100 inches");
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve textured face");
    REQUIRE_SDK(fixture, SUFaceGetFrontMaterial(face, &face_material));
    REQUIRE(fixture, material_name(face_material, name),
            "could not read textured face material");
    REQUIRE(fixture, strcmp(name, "TexturedMaterial") == 0,
            "face does not reference TexturedMaterial");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("textures", validate_textures)
