// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/geometry_input.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/texture.h>
#include <stdio.h>

static bool set_positioned_material(
    SUGeometryInputRef geometry,
    size_t face_index,
    SUMaterialRef material,
    const SUPoint2D uv[4],
    const SUPoint3D points[4],
    SUVector3D projection) {
    struct SUMaterialPositionInput input = {0};
    input.num_uv_coords = 4;
    input.material = material;
    input.projection = projection;
    for (size_t index = 0; index < 4; ++index) {
        input.uv_coords[index] = uv[index];
        input.points[index] = points[index];
    }
    return SUGeometryInputFaceSetFrontMaterialByPosition(
               geometry, face_index, &input) == SU_ERROR_NONE;
}

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion version = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUMaterialRef material = SU_INVALID;
    SUMaterialCreate(&material);
    SUMaterialSetName(material, "UVConformanceMaterial");

    SUTextureRef texture = SU_INVALID;
    // The fixture validates UV transforms, not image decoding. Keeping a tiny
    // image inline makes every version-matrix run deterministic and avoids a
    // hidden dependency on an untracked resources directory.
    const SUByte pixels[16] = {
        255, 0,   0,   255, 0,   255, 0,   255,
        0,   0,   255, 255, 255, 255, 255, 255,
    };
    if (SUTextureCreateFromImageData(&texture, 2, 2, 32, pixels) != SU_ERROR_NONE) {
        fprintf(stderr, "Failed to create UV conformance texture\n");
        SUModelRelease(&model);
        term_sketchup();
        return 1;
    }
    SUMaterialSetTexture(material, texture);
    SUModelAddMaterials(model, 1, &material);

    const SUPoint3D vertices[16] = {
        {0, 0, 0}, {20, 0, 0}, {20, 20, 0}, {0, 20, 0},
        {30, 0, 0}, {50, 0, 0}, {50, 20, 0}, {30, 20, 0},
        {60, 0, 0}, {80, 0, 0}, {80, 20, 0}, {60, 20, 0},
        {90, 0, 0}, {110, 0, 0}, {110, 0, 20}, {90, 0, 20},
    };
    const SUPoint2D uv[4][4] = {
        {{0, 0}, {2, 0}, {2, 1}, {0, 1}},
        {{0, 1}, {0, 0}, {1, 0}, {1, 1}},
        {{1, 0}, {0, 0}, {0, 1}, {1, 1}},
        {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
    };

    SUGeometryInputRef geometry = SU_INVALID;
    SUGeometryInputCreate(&geometry);
    SUGeometryInputSetVertices(geometry, 16, vertices);

    for (size_t face = 0; face < 4; ++face) {
        SULoopInputRef loop = SU_INVALID;
        SULoopInputCreate(&loop);
        for (size_t corner = 0; corner < 4; ++corner) {
            SULoopInputAddVertexIndex(loop, face * 4 + corner);
        }
        size_t face_index = 0;
        if (SUGeometryInputAddFace(geometry, &loop, &face_index) != SU_ERROR_NONE) {
            fprintf(stderr, "Failed to add UV conformance face\n");
            SUGeometryInputRelease(&geometry);
            SUModelRelease(&model);
            term_sketchup();
            return 1;
        }

        SUVector3D projection = {0, 0, 0};
        if (face == 3) projection = (SUVector3D){0, -1, 0};
        if (!set_positioned_material(
                geometry,
                face_index,
                material,
                &uv[face][0],
                &vertices[face * 4],
                projection)) {
            fprintf(stderr, "Failed to position UV conformance material\n");
            SUGeometryInputRelease(&geometry);
            SUModelRelease(&model);
            term_sketchup();
            return 1;
        }
    }

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);
    if (SUEntitiesFill(entities, geometry, true) != SU_ERROR_NONE) {
        fprintf(stderr, "Failed to fill UV conformance geometry\n");
        SUGeometryInputRelease(&geometry);
        SUModelRelease(&model);
        term_sketchup();
        return 1;
    }

    bool saved = save_model_with_version(model, "data/uv_positions.skp", version);
    SUGeometryInputRelease(&geometry);
    SUModelRelease(&model);
    term_sketchup();
    return saved ? 0 : 1;
}
