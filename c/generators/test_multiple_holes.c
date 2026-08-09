// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/geometry_input.h>
#include <SketchUpAPI/model/model.h>
#include <stdio.h>

static void add_loop_indices(SULoopInputRef loop, const size_t indices[4]) {
    for (size_t i = 0; i < 4; ++i) {
        SULoopInputAddVertexIndex(loop, indices[i]);
    }
}

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion version = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);
    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    const SUPoint3D vertices[16] = {
        {0, 0, 0}, {120, 0, 0}, {120, 80, 0}, {0, 80, 0},
        {10, 10, 0}, {30, 10, 0}, {30, 30, 0}, {10, 30, 0},
        {50, 20, 0}, {70, 20, 0}, {70, 60, 0}, {50, 60, 0},
        {90, 50, 0}, {110, 50, 0}, {110, 70, 0}, {90, 70, 0},
    };
    const size_t outer_indices[4] = {0, 1, 2, 3};
    const size_t hole_indices[3][4] = {
        {7, 6, 5, 4},
        {11, 10, 9, 8},
        {15, 14, 13, 12},
    };

    SUGeometryInputRef geometry = SU_INVALID;
    SUGeometryInputCreate(&geometry);
    SUGeometryInputSetVertices(geometry, 16, vertices);

    SULoopInputRef outer = SU_INVALID;
    SULoopInputCreate(&outer);
    add_loop_indices(outer, outer_indices);
    size_t face_index = 0;
    if (SUGeometryInputAddFace(geometry, &outer, &face_index) != SU_ERROR_NONE) {
        fprintf(stderr, "Could not add the outer loop\n");
        return 1;
    }

    for (size_t hole = 0; hole < 3; ++hole) {
        SULoopInputRef inner = SU_INVALID;
        SULoopInputCreate(&inner);
        add_loop_indices(inner, hole_indices[hole]);
        if (SUGeometryInputFaceAddInnerLoop(geometry, face_index, &inner) !=
            SU_ERROR_NONE) {
            fprintf(stderr, "Could not add inner loop %zu\n", hole);
            return 1;
        }
    }

    if (SUEntitiesFill(entities, geometry, true) != SU_ERROR_NONE) {
        fprintf(stderr, "Could not fill entities with the holed face\n");
        return 1;
    }
    SUGeometryInputRelease(&geometry);

    bool saved = save_model_with_version(model, "data/multiple_holes.skp", version);
    SUModelRelease(&model);
    term_sketchup();
    return saved ? 0 : 1;
}
