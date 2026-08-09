// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/geometry.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    SUPoint3D start = { 0, 0, 0 };
    SUPoint3D end = { 10, 10, 10 };
    SUEdgeRef edge = SU_INVALID;
    SUEdgeCreate(&edge, &start, &end);

    SUEntitiesAddEdges(entities, 1, &edge);

    save_model_with_version(model, "data/edge.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
