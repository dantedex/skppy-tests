// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/texture.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/model/geometry_input.h>
#include <SketchUpAPI/geometry.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    // 1. Create Materials with textures
    SUMaterialRef mat_front = SU_INVALID;
    SUMaterialCreate(&mat_front);
    SUMaterialSetName(mat_front, "FrontMaterial");
    
    SUMaterialRef mat_back = SU_INVALID;
    SUMaterialCreate(&mat_back);
    SUMaterialSetName(mat_back, "BackMaterial");

    SUTextureRef tex_front = SU_INVALID;
    if (SUTextureCreateFromFile(&tex_front, "resources/studio.exr", 1.0, 1.0) == SU_ERROR_NONE) {
        SUMaterialSetTexture(mat_front, tex_front);
    }
    
    SUTextureRef tex_back = SU_INVALID;
    // Using same texture for simplicity, but different mapping
    if (SUTextureCreateFromFile(&tex_back, "resources/studio.exr", 1.0, 1.0) == SU_ERROR_NONE) {
        SUMaterialSetTexture(mat_back, tex_back);
    }

    SUModelAddMaterials(model, 1, &mat_front);
    SUModelAddMaterials(model, 1, &mat_back);

    // 2. Use GeometryInput to create face with UVs and back material
    SUGeometryInputRef geom_input = SU_INVALID;
    SUGeometryInputCreate(&geom_input);

    SUPoint3D vertices[4] = {
        { 0, 0, 0 }, { 100, 0, 0 }, { 100, 100, 0 }, { 0, 100, 0 }
    };
    SUGeometryInputSetVertices(geom_input, 4, vertices);

    SULoopInputRef loop = SU_INVALID;
    SULoopInputCreate(&loop);
    for (int i = 0; i < 4; ++i) SULoopInputAddVertexIndex(loop, i);

    size_t face_index;
    SUGeometryInputAddFace(geom_input, &loop, &face_index);

    // Front Material + UVs
    struct SUMaterialInput front_mat_input = {0};
    front_mat_input.material = mat_front;
    front_mat_input.num_uv_coords = 4;
    for (int i = 0; i < 4; ++i) {
        front_mat_input.vertex_indices[i] = i;
    }
    front_mat_input.uv_coords[0] = (SUPoint2D){0.0, 0.0};
    front_mat_input.uv_coords[1] = (SUPoint2D){1.0, 0.0};
    front_mat_input.uv_coords[2] = (SUPoint2D){1.0, 1.0};
    front_mat_input.uv_coords[3] = (SUPoint2D){0.0, 1.0};
    SUGeometryInputFaceSetFrontMaterial(geom_input, face_index, &front_mat_input);

    // Back Material + UVs (rotated mapping for distinction)
    struct SUMaterialInput back_mat_input = {0};
    back_mat_input.material = mat_back;
    back_mat_input.num_uv_coords = 4;
    for (int i = 0; i < 4; ++i) {
        back_mat_input.vertex_indices[i] = i;
    }
    back_mat_input.uv_coords[0] = (SUPoint2D){0.0, 1.0};
    back_mat_input.uv_coords[1] = (SUPoint2D){0.0, 0.0};
    back_mat_input.uv_coords[2] = (SUPoint2D){1.0, 0.0};
    back_mat_input.uv_coords[3] = (SUPoint2D){1.0, 1.0};
    SUGeometryInputFaceSetBackMaterial(geom_input, face_index, &back_mat_input);

    SUEntitiesFill(entities, geom_input, true);

    save_model_with_version(model, "data/uv_back_mat.skp", ver);

    SUGeometryInputRelease(&geom_input);
    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
