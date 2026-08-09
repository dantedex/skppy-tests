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

    // Outer square 100x100 and inner square 50x50 (hole).
    SUPoint3D all_v[8] = {
        {0,0,0}, {100,0,0}, {100,100,0}, {0,100,0},       // outer
        {25,25,0}, {75,25,0}, {75,75,0}, {25,75,0}        // inner
    };

    SUGeometryInputRef geom = SU_INVALID;
    SUGeometryInputCreate(&geom);
    SUGeometryInputSetVertices(geom, 8, all_v);

    // Outer loop: indices 0..3
    SULoopInputRef outer_loop = SU_INVALID;
    SULoopInputCreate(&outer_loop);
    for (int i = 0; i < 4; i++) SULoopInputAddVertexIndex(outer_loop, i);
    size_t face_index;
    enum SUResult res = SUGeometryInputAddFace(geom, &outer_loop, &face_index);
    if (res != SU_ERROR_NONE) {
        fprintf(stderr, "SUGeometryInputAddFace failed (error %d)\n", res);
    }

    // Inner loop (hole): indices 4..7, reversed winding
    SULoopInputRef inner_loop = SU_INVALID;
    SULoopInputCreate(&inner_loop);
    for (int i = 3; i >= 0; i--) SULoopInputAddVertexIndex(inner_loop, i + 4);
    res = SUGeometryInputFaceAddInnerLoop(geom, face_index, &inner_loop);
    if (res != SU_ERROR_NONE) {
        fprintf(stderr, "SUGeometryInputFaceAddInnerLoop failed (error %d)\n", res);
    }

    SUEntitiesFill(entities, geom, true);
    SUGeometryInputRelease(&geom);

    save_model_with_version(model, "data/face_hole.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
