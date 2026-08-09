// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_materials(const char* directory) {
    const char* fixture = "materials";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUMaterialRef material = SU_INVALID;
    SUMaterialRef face_material = SU_INVALID;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SUColor color = {0};
    char name[NAME_CAPACITY] = {0};
    size_t faces = 0;
    size_t edges = 0;

    opened = open_fixture(fixture, directory, "materials.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture, find_material(model, "RedMaterial", &material),
            "could not find RedMaterial");
    REQUIRE_SDK(fixture, SUMaterialGetColor(material, &color));
    REQUIRE(fixture, color.red == 255 && color.green == 0 && color.blue == 0,
            "RedMaterial color was not preserved");
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &edges));
    REQUIRE(fixture, faces == 1 && edges == 4,
            "expected one rectangular face and four edges");
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve material face");
    REQUIRE_SDK(fixture, SUFaceGetFrontMaterial(face, &face_material));
    REQUIRE(fixture, material_name(face_material, name),
            "could not read face material name");
    REQUIRE(fixture, strcmp(name, "RedMaterial") == 0,
            "face does not reference RedMaterial");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("materials", validate_materials)
