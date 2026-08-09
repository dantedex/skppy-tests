// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_solids(const char* directory) {
    const char* fixture = "solids";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "solids.skp", &model);
    bool valid = false;
    SUEntitiesRef root = SU_INVALID;
    SUEntitiesRef solid = SU_INVALID;
    SUGroupRef group = SU_INVALID;
    size_t groups = 0;
    size_t edges = 0;
    size_t faces = 0;
    size_t retrieved = 0;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &root));
    REQUIRE_SDK(fixture, SUEntitiesGetNumGroups(root, &groups));
    REQUIRE(fixture, groups == 1, "expected one solid group");
    REQUIRE_SDK(fixture, SUEntitiesGetGroups(root, 1, &group, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve solid group");
    REQUIRE_SDK(fixture, SUGroupGetEntities(group, &solid));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(solid, false, &edges));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(solid, &faces));
    REQUIRE(fixture, edges == 12 && faces == 6, "solid group is not a closed cube");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("solids", validate_solids)
