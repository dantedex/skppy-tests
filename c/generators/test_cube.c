// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
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

    // A simple 10x10x10 cube at the origin, built with SUGeometryInput
    // so vertices are shared (8 verts, 12 edges, 6 faces).
    SUPoint3D v[8] = {
        {0,0,0}, {10,0,0}, {10,10,0}, {0,10,0},
        {0,0,10}, {10,0,10}, {10,10,10}, {0,10,10}
    };

    SUGeometryInputRef geom = SU_INVALID;
    SUGeometryInputCreate(&geom);
    SUGeometryInputSetVertices(geom, 8, v);

    int faces[6][4] = {
        {3, 2, 1, 0}, {4, 5, 6, 7}, {0, 1, 5, 4},
        {2, 3, 7, 6}, {1, 2, 6, 5}, {3, 0, 4, 7},
    };
    for (int f = 0; f < 6; ++f) {
        SULoopInputRef loop = SU_INVALID;
        SULoopInputCreate(&loop);
        for (int i = 0; i < 4; ++i) SULoopInputAddVertexIndex(loop, faces[f][i]);
        size_t idx;
        SUGeometryInputAddFace(geom, &loop, &idx);
    }

    SUEntitiesFill(entities, geom, true);
    SUGeometryInputRelease(&geom);

    save_model_with_version(model, "data/cube.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
