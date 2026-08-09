// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/arccurve.h>
#include <SketchUpAPI/geometry.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    // A quarter circle (90-degree arc) in the XY plane.
    SUPoint3D center = { 0, 0, 0 };
    SUPoint3D start  = { 10, 0, 0 };   // 0 degrees
    SUPoint3D end    = { 0, 10, 0 };   // 90 degrees
    SUVector3D normal = { 0, 0, 1 };
    size_t num_segments = 6;
    SUArcCurveRef arc = SU_INVALID;
    SUArcCurveCreate(&arc, &center, &start, &end, &normal, num_segments);
    SUEntitiesAddArcCurves(entities, 1, &arc);

    save_model_with_version(model, "data/quarter_circle.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
