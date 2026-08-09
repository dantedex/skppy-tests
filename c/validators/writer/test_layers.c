// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_layers(const char* directory) {
    const char* fixture = "layers";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SULayerRef walls = SU_INVALID;
    SULayerRef roof = SU_INVALID;
    SULayerRef active = SU_INVALID;
    SULayerRef member = SU_INVALID;
    SULayerRef entity_layer = SU_INVALID;
    SUMaterialRef layer_material = SU_INVALID;
    SULayerFolderRef building = SU_INVALID;
    SULayerFolderRef upper = SU_INVALID;
    SULayerFolderRef parent = SU_INVALID;
    SUColor layer_color = {0};
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SUEdgeRef edge = SU_INVALID;
    size_t layer_count = 0;
    size_t root_folder_count = 0;
    size_t child_folder_count = 0;
    size_t member_count = 0;
    size_t retrieved = 0;
    bool visible = false;
    int major = 0;
    int minor = 0;
    int build = 0;
    char name[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "layers.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE_SDK(fixture, SUModelGetNumLayers(model, &layer_count));
    REQUIRE(fixture, layer_count == (major <= 17 ? 3 : 2), "unexpected layer count");
    REQUIRE(fixture, find_layer(model, "Writer Walls", &walls),
            "could not find Writer Walls");
    REQUIRE(fixture, find_layer(model, "Writer Roof", &roof),
            "could not find Writer Roof");
    REQUIRE_SDK(fixture, SULayerGetVisibility(walls, &visible));
    REQUIRE(fixture, visible, "Writer Walls should be visible");
    REQUIRE_SDK(fixture, SULayerGetVisibility(roof, &visible));
    REQUIRE(
        fixture,
        major <= 17 ? visible : !visible,
        "Writer Roof visibility does not match the format behavior");
    REQUIRE_SDK(fixture, SULayerGetMaterial(walls, &layer_material));
    REQUIRE(fixture, material_name(layer_material, name),
            "could not read Writer Walls display material name");
    REQUIRE(fixture, strcmp(name, "Writer Layer Color") == 0,
            "Writer Walls display material name was not preserved");
    REQUIRE_SDK(fixture, SUMaterialGetColor(layer_material, &layer_color));
    REQUIRE(fixture,
            layer_color.red == 10 && layer_color.green == 20 && layer_color.blue == 30,
            "Writer Walls display material color was not preserved");
    REQUIRE_SDK(fixture, SUModelGetActiveLayer(model, &active));
    REQUIRE(fixture, layer_name(active, name), "could not read active layer name");
    REQUIRE(fixture, strcmp(name, "Writer Roof") == 0,
            "Writer Roof is not the active layer");

    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve layered face");
    REQUIRE_SDK(
        fixture, SUDrawingElementGetLayer(SUFaceToDrawingElement(face), &entity_layer));
    REQUIRE(fixture, layer_name(entity_layer, name),
            "could not read face layer name");
    REQUIRE(fixture, strcmp(name, "Writer Walls") == 0,
            "face is not owned by Writer Walls");
    retrieved = 0;
    REQUIRE_SDK(fixture, SUEntitiesGetEdges(entities, false, 1, &edge, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve layered edge");
    REQUIRE_SDK(
        fixture, SUDrawingElementGetLayer(SUEdgeToDrawingElement(edge), &entity_layer));
    REQUIRE(fixture, layer_name(entity_layer, name),
            "could not read edge layer name");
    REQUIRE(fixture, strcmp(name, "Writer Roof") == 0,
            "first edge is not owned by Writer Roof");

    REQUIRE_SDK(fixture, SUModelGetNumLayerFolders(model, &root_folder_count));
    if (major <= 17) {
        REQUIRE(fixture, root_folder_count == 0, "legacy model unexpectedly exposed layer folders");
        valid = true;
        goto cleanup;
    }
    REQUIRE(fixture, root_folder_count == 1, "expected one root layer folder");
    REQUIRE_SDK(
        fixture, SUModelGetLayerFolders(model, 1, &building, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve root layer folder");
    REQUIRE(fixture, folder_name(building, name),
            "could not read root layer folder name");
    REQUIRE(fixture, strcmp(name, "Writer Building") == 0,
            "root layer folder name was not preserved");
    REQUIRE_SDK(fixture, SULayerFolderGetNumLayers(building, &member_count));
    REQUIRE(fixture, member_count == 1, "Writer Building should contain one layer");
    retrieved = 0;
    REQUIRE_SDK(fixture, SULayerFolderGetLayers(building, 1, &member, &retrieved));
    REQUIRE(fixture, retrieved == 1 && member.ptr == walls.ptr,
            "Writer Walls is not in Writer Building");

    REQUIRE_SDK(
        fixture, SULayerFolderGetNumLayerFolders(building, &child_folder_count));
    REQUIRE(fixture, child_folder_count == 1,
            "Writer Building should contain one child folder");
    retrieved = 0;
    REQUIRE_SDK(
        fixture, SULayerFolderGetLayerFolders(building, 1, &upper, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve child layer folder");
    REQUIRE(fixture, folder_name(upper, name),
            "could not read child layer folder name");
    REQUIRE(fixture, strcmp(name, "Writer Upper") == 0,
            "child layer folder name was not preserved");
    REQUIRE_SDK(fixture, SULayerFolderGetParentLayerFolder(upper, &parent));
    REQUIRE(fixture, parent.ptr == building.ptr,
            "Writer Upper does not reference Writer Building as parent");
    REQUIRE_SDK(fixture, SULayerFolderGetNumLayers(upper, &member_count));
    REQUIRE(fixture, member_count == 1, "Writer Upper should contain one layer");
    retrieved = 0;
    REQUIRE_SDK(fixture, SULayerFolderGetLayers(upper, 1, &member, &retrieved));
    REQUIRE(fixture, retrieved == 1 && member.ptr == roof.ptr,
            "Writer Roof is not in Writer Upper");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("layers", validate_layers)
