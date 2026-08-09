// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/color.h>
#include <SketchUpAPI/geometry.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    // Create a face
    SUPoint3D vertices[4] = {
        { 0, 0, 0 }, { 10, 0, 0 }, { 10, 10, 0 }, { 0, 10, 0 }
    };
    SULoopInputRef loop = SU_INVALID;
    SULoopInputCreate(&loop);
    for (int i = 0; i < 4; ++i) SULoopInputAddVertexIndex(loop, i);
    SUFaceRef face = SU_INVALID;
    SUFaceCreate(&face, vertices, &loop);
    SUEntitiesAddFaces(entities, 1, &face);

    // Create a material
    SUMaterialRef material = SU_INVALID;
    SUMaterialCreate(&material);
    SUMaterialSetName(material, "RedMaterial");
    SUColor red = { 255, 0, 0, 255 };
    SUMaterialSetColor(material, &red);
    SUModelAddMaterials(model, 1, &material);

    // Apply material to face
    SUFaceSetFrontMaterial(face, material);

    save_model_with_version(model, "data/materials.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
