// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/model.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion version = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);
    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    const SUPoint3D face_vertices[4] = {
        {1000000.125, -1000000.25, 12345.5},
        {1000010.625, -1000000.25, 12345.5},
        {1000010.625, -999989.75, 12345.5},
        {1000000.125, -999989.75, 12345.5},
    };
    SULoopInputRef loop = SU_INVALID;
    SULoopInputCreate(&loop);
    for (size_t i = 0; i < 4; ++i) SULoopInputAddVertexIndex(loop, i);
    SUFaceRef face = SU_INVALID;
    if (SUFaceCreate(&face, face_vertices, &loop) != SU_ERROR_NONE) {
        fprintf(stderr, "Could not create the high-coordinate face\n");
        return 1;
    }
    SUEntitiesAddFaces(entities, 1, &face);

    const SUPoint3D edge_start = {-1000000.5, 0.0009765625, -0.333333333333};
    const SUPoint3D edge_end = {-999999.875, 0.001953125, 0.666666666667};
    SUEdgeRef edge = SU_INVALID;
    if (SUEdgeCreate(&edge, &edge_start, &edge_end) != SU_ERROR_NONE) {
        fprintf(stderr, "Could not create the precision edge\n");
        return 1;
    }
    SUEntitiesAddEdges(entities, 1, &edge);

    bool saved = save_model_with_version(
        model, "data/extreme_coordinates.skp", version);
    SUModelRelease(&model);
    term_sketchup();
    return saved ? 0 : 1;
}
