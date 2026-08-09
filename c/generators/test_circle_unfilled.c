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

    // An unfilled circle: just the ArcCurve (edges only, no face).
    SUPoint3D center = { 0, 0, 0 };
    SUVector3D normal = { 0, 0, 1 };
    SUPoint3D start_point = { 10, 0, 0 };
    size_t num_segments = 24;
    SUArcCurveRef arc = SU_INVALID;
    SUArcCurveCreate(&arc, &center, &start_point, &start_point, &normal, num_segments);
    SUEntitiesAddArcCurves(entities, 1, &arc);

    save_model_with_version(model, "data/circle_unfilled.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
