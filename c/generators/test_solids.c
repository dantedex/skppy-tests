// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/group.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/geometry_input.h>
#include <SketchUpAPI/geometry.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef model_entities = SU_INVALID;
    SUModelGetEntities(model, &model_entities);

    SUGroupRef group = SU_INVALID;
    SUGroupCreate(&group);
    SUEntitiesAddGroup(model_entities, group);

    SUEntitiesRef group_entities = SU_INVALID;
    SUGroupGetEntities(group, &group_entities);

    // Build a closed cube using SUGeometryInput so vertices are shared
    // (the recommended path per the SUFaceCreate API docs).
    SUPoint3D v[8] = {
        {0,0,0}, {10,0,0}, {10,10,0}, {0,10,0},
        {0,0,10}, {10,0,10}, {10,10,10}, {0,10,10}
    };

    SUGeometryInputRef geom = SU_INVALID;
    SUGeometryInputCreate(&geom);
    SUGeometryInputSetVertices(geom, 8, v);

    // 6 quad faces, CCW when viewed from outside.
    int faces[6][4] = {
        {3, 2, 1, 0}, // bottom  (z=0, normal -Z)
        {4, 5, 6, 7}, // top     (z=10, normal +Z)
        {0, 1, 5, 4}, // front   (y=0, normal -Y)
        {2, 3, 7, 6}, // back    (y=10, normal +Y)
        {1, 2, 6, 5}, // right   (x=10, normal +X)
        {3, 0, 4, 7}, // left    (x=0, normal -X)
    };

    for (int f = 0; f < 6; ++f) {
        SULoopInputRef loop = SU_INVALID;
        SULoopInputCreate(&loop);
        for (int i = 0; i < 4; ++i) {
            SULoopInputAddVertexIndex(loop, faces[f][i]);
        }
        size_t face_index;
        enum SUResult res = SUGeometryInputAddFace(geom, &loop, &face_index);
        if (res != SU_ERROR_NONE) {
            fprintf(stderr, "SUGeometryInputAddFace failed on face %d (error %d)\n", f, res);
        }
    }

    SUEntitiesFill(group_entities, geom, true);
    SUGeometryInputRelease(&geom);

    save_model_with_version(model, "data/solids.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
