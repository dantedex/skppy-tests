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

    // A 4-sided pyramid: square base + 4 triangular sides meeting at apex.
    SUPoint3D v[5] = {
        {0,0,0}, {10,0,0}, {10,10,0}, {0,10,0},  // base
        {5,5,10}                                    // apex
    };

    SUGeometryInputRef geom = SU_INVALID;
    SUGeometryInputCreate(&geom);
    SUGeometryInputSetVertices(geom, 5, v);

    // Base (normal -Z)
    {
        SULoopInputRef loop = SU_INVALID;
        SULoopInputCreate(&loop);
        SULoopInputAddVertexIndex(loop, 3);
        SULoopInputAddVertexIndex(loop, 2);
        SULoopInputAddVertexIndex(loop, 1);
        SULoopInputAddVertexIndex(loop, 0);
        size_t idx;
        SUGeometryInputAddFace(geom, &loop, &idx);
    }
    // 4 triangular sides
    int tris[4][3] = {
        {0, 1, 4}, {1, 2, 4}, {2, 3, 4}, {3, 0, 4}
    };
    for (int i = 0; i < 4; i++) {
        SULoopInputRef loop = SU_INVALID;
        SULoopInputCreate(&loop);
        for (int j = 0; j < 3; j++) SULoopInputAddVertexIndex(loop, tris[i][j]);
        size_t idx;
        SUGeometryInputAddFace(geom, &loop, &idx);
    }

    SUEntitiesFill(entities, geom, true);
    SUGeometryInputRelease(&geom);

    save_model_with_version(model, "data/pyramid.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
