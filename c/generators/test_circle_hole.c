// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/geometry_input.h>
#include <SketchUpAPI/geometry.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    // A filled circle (disk) with a smaller circular hole.
    const int segs = 24;
    const double r_outer = 20.0;
    const double r_inner = 5.0;
    SUPoint3D v[segs * 2];
    for (int i = 0; i < segs; i++) {
        double a = 2.0 * 3.14159265358979 * i / segs;
        v[i]        = (SUPoint3D){ r_outer * cos(a), r_outer * sin(a), 0.0 };
        v[i + segs] = (SUPoint3D){ r_inner * cos(a), r_inner * sin(a), 0.0 };
    }

    SUGeometryInputRef geom = SU_INVALID;
    SUGeometryInputCreate(&geom);
    SUGeometryInputSetVertices(geom, segs * 2, v);

    // Outer loop
    SULoopInputRef outer = SU_INVALID;
    SULoopInputCreate(&outer);
    for (int i = 0; i < segs; i++) SULoopInputAddVertexIndex(outer, i);
    size_t face_idx;
    SUGeometryInputAddFace(geom, &outer, &face_idx);

    // Inner loop (hole) - reversed winding
    SULoopInputRef inner = SU_INVALID;
    SULoopInputCreate(&inner);
    for (int i = segs - 1; i >= 0; i--) SULoopInputAddVertexIndex(inner, i + segs);
    SUGeometryInputFaceAddInnerLoop(geom, face_idx, &inner);

    SUEntitiesFill(entities, geom, true);
    SUGeometryInputRelease(&geom);

    save_model_with_version(model, "data/circle_hole.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
