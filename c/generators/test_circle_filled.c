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

    // A filled circle (disk) created as a 24-segment polygon face.
    const int segs = 24;
    const double r = 10.0;
    SUPoint3D v[segs];
    for (int i = 0; i < segs; i++) {
        double a = 2.0 * 3.14159265358979 * i / segs;
        v[i] = (SUPoint3D){ r * cos(a), r * sin(a), 0.0 };
    }

    SUGeometryInputRef geom = SU_INVALID;
    SUGeometryInputCreate(&geom);
    SUGeometryInputSetVertices(geom, segs, v);
    SULoopInputRef loop = SU_INVALID;
    SULoopInputCreate(&loop);
    for (int i = 0; i < segs; i++) SULoopInputAddVertexIndex(loop, i);
    size_t idx;
    SUGeometryInputAddFace(geom, &loop, &idx);
    SUEntitiesFill(entities, geom, true);
    SUGeometryInputRelease(&geom);

    save_model_with_version(model, "data/circle_filled.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
