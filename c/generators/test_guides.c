// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/guide_point.h>
#include <SketchUpAPI/model/guide_line.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    // Create a Guide Point
    SUGuidePointRef gp = SU_INVALID;
    SUPoint3D pos = { 10, 20, 30 };
    SUGuidePointCreate(&gp, &pos);
    SUEntitiesAddGuidePoints(entities, 1, &gp);

    // Create a Guide Line
    SUGuideLineRef gl = SU_INVALID;
    SUPoint3D p1 = { 0, 0, 0 };
    SUPoint3D p2 = { 100, 100, 100 };
    SUGuideLineCreateFinite(&gl, &p1, &p2);
    SUEntitiesAddGuideLines(entities, 1, &gl);

    save_model_with_version(model, "data/guides.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
