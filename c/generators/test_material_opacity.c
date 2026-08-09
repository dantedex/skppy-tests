// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/model/model.h>

static SUFaceRef create_face(double x_offset) {
    const SUPoint3D vertices[4] = {
        {x_offset, 0, 0},
        {x_offset + 10, 0, 0},
        {x_offset + 10, 10, 0},
        {x_offset, 10, 0},
    };
    SULoopInputRef loop = SU_INVALID;
    SULoopInputCreate(&loop);
    for (size_t index = 0; index < 4; ++index) {
        SULoopInputAddVertexIndex(loop, index);
    }
    SUFaceRef face = SU_INVALID;
    SUFaceCreate(&face, vertices, &loop);
    return face;
}

static SUMaterialRef create_material(
    const char* name,
    SUColor color,
    double opacity,
    bool use_opacity) {
    SUMaterialRef material = SU_INVALID;
    SUMaterialCreate(&material);
    SUMaterialSetName(material, name);
    SUMaterialSetColor(material, &color);
    SUMaterialSetOpacity(material, opacity);
    SUMaterialSetUseOpacity(material, use_opacity);
    return material;
}

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion version = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);
    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    SUFaceRef faces[3] = {
        create_face(0), create_face(20), create_face(40),
    };
    SUEntitiesAddFaces(entities, 3, faces);

    SUMaterialRef materials[3] = {
        create_material(
            "OpacityIgnored", (SUColor){220, 40, 40, 255}, 0.25, false),
        create_material(
            "Translucent", (SUColor){40, 220, 40, 255}, 0.35, true),
        create_material(
            "FullyTransparent", (SUColor){40, 40, 220, 255}, 0.0, true),
    };
    SUModelAddMaterials(model, 3, materials);
    for (size_t index = 0; index < 3; ++index) {
        SUFaceSetFrontMaterial(faces[index], materials[index]);
    }

    bool saved = save_model_with_version(model, "data/material_opacity.skp", version);
    SUModelRelease(&model);
    term_sketchup();
    return saved ? 0 : 1;
}
