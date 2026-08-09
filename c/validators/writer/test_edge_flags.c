// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_edge_flags(const char* directory) {
    const char* fixture = "edge_flags";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "edge_flags.skp", &model);
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUEdgeRef edges[4] = {SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID};
    size_t retrieved = 0;
    bool soft = false;
    bool smooth = false;
    bool hidden = false;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetEdges(entities, false, 4, edges, &retrieved));
    REQUIRE(fixture, retrieved == 4, "could not retrieve four flagged edges");
    REQUIRE_SDK(fixture, SUEdgeGetSoft(edges[1], &soft));
    REQUIRE_SDK(fixture, SUEdgeGetSmooth(edges[2], &smooth));
    REQUIRE_SDK(
        fixture, SUDrawingElementGetHidden(SUEdgeToDrawingElement(edges[3]), &hidden));
    REQUIRE(fixture, soft && smooth && hidden, "edge flags were not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("edge_flags", validate_edge_flags)
