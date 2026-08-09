// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_nested_components(const char* directory) {
    const char* fixture = "nested_components";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "nested_components.skp", &model);
    bool valid = false;
    SUComponentDefinitionRef parent = SU_INVALID;
    SUComponentDefinitionRef child = SU_INVALID;
    SUEntitiesRef parent_entities = SU_INVALID;
    SUEntitiesRef child_entities = SU_INVALID;
    size_t instances = 0;
    size_t edges = 0;
    if (!opened) return false;
    REQUIRE(fixture, find_definition(model, "ParentComponent", &parent), "missing parent definition");
    REQUIRE(fixture, find_definition(model, "SubComponent", &child), "missing child definition");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetEntities(parent, &parent_entities));
    REQUIRE_SDK(fixture, SUComponentDefinitionGetEntities(child, &child_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(parent_entities, &instances));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(child_entities, false, &edges));
    REQUIRE(fixture, instances == 1 && edges == 1, "nested definition structure was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("nested_components", validate_nested_components)
