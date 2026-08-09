// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/drawing_element.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/model.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion version = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);
    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    SUEdgeRef edges[4] = {SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID};
    for (size_t index = 0; index < 4; ++index) {
        SUPoint3D start = {0, (double)index * 10.0, 0};
        SUPoint3D end = {5, (double)index * 10.0, 0};
        if (SUEdgeCreate(&edges[index], &start, &end) != SU_ERROR_NONE) {
            return 1;
        }
    }

    SUEdgeSetSoft(edges[1], true);
    SUEdgeSetSmooth(edges[2], true);
    SUDrawingElementSetHidden(SUEdgeToDrawingElement(edges[3]), true);

    if (SUEntitiesAddEdges(entities, 4, edges) != SU_ERROR_NONE) return 1;

    bool saved = save_model_with_version(model, "data/edge_flags.skp", version);
    SUModelRelease(&model);
    term_sketchup();
    return saved ? 0 : 1;
}
