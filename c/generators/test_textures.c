// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/texture.h>
#include <SketchUpAPI/model/material.h>
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
        { 0, 0, 0 }, { 100, 0, 0 }, { 100, 100, 0 }, { 0, 100, 0 }
    };
    SULoopInputRef loop = SU_INVALID;
    SULoopInputCreate(&loop);
    for (int i = 0; i < 4; ++i) SULoopInputAddVertexIndex(loop, i);
    SUFaceRef face = SU_INVALID;
    SUFaceCreate(&face, vertices, &loop);
    SUEntitiesAddFaces(entities, 1, &face);

    // Create a material with texture
    SUMaterialRef material = SU_INVALID;
    SUMaterialCreate(&material);
    SUMaterialSetName(material, "TexturedMaterial");
    
    SUTextureRef texture = SU_INVALID;
    // We assume there's a texture file available or use a relative path
    // For this test, we just try to create it from a dummy path if it exists
    // or we might need a real .png/.jpg.
    // SUTextureCreateFromFile(&texture, "resources/brick_wall/Color.jpg", 1.0);
    // If we don't have a real file, we can use ImageRep to create a procedural one,
    // but that's more complex. Let's assume a resource exists.
    
    enum SUResult res = SUTextureCreateFromFile(&texture, "resources/brick_wall/brick_wall_21_basecolor.png", 1.0, 1.0);
    if (res == SU_ERROR_NONE) {
        SUMaterialSetTexture(material, texture);
    } else {
        fprintf(stderr, "Failed to load texture (error code: %d)\n", res);
    }

    SUModelAddMaterials(model, 1, &material);
    SUFaceSetFrontMaterial(face, material);

    save_model_with_version(model, "data/textures.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
