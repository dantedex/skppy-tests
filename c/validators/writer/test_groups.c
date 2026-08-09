// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_groups(const char* directory) {
    const char* fixture = "groups";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "groups.skp", &model);
    bool valid = false;
    SUEntitiesRef root = SU_INVALID;
    SUGroupRef groups[3] = {SU_INVALID, SU_INVALID, SU_INVALID};
    size_t count = 0;
    size_t retrieved = 0;
    bool face_ok = false;
    bool nested_ok = false;
    bool component_ok = false;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &root));
    REQUIRE_SDK(fixture, SUEntitiesGetNumGroups(root, &count));
    REQUIRE(fixture, count == 3, "expected three root groups");
    REQUIRE_SDK(fixture, SUEntitiesGetGroups(root, 3, groups, &retrieved));
    REQUIRE(fixture, retrieved == 3, "could not retrieve root groups");
    for (size_t index = 0; index < retrieved; ++index) {
        char name[NAME_CAPACITY] = {0};
        SUEntitiesRef entities = SU_INVALID;
        REQUIRE(fixture, group_name(groups[index], name), "could not read group name");
        REQUIRE_SDK(fixture, SUGroupGetEntities(groups[index], &entities));
        if (strcmp(name, "Face Group") == 0) {
            size_t faces = 0;
            REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
            face_ok = faces == 1;
        } else if (strcmp(name, "Outer Group") == 0) {
            size_t nested = 0;
            REQUIRE_SDK(fixture, SUEntitiesGetNumGroups(entities, &nested));
            nested_ok = nested == 1;
        } else if (strcmp(name, "Component Group") == 0) {
            size_t instances = 0;
            REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(entities, &instances));
            component_ok = instances == 1;
        }
    }
    REQUIRE(fixture, face_ok && nested_ok && component_ok, "group definition contents were not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("groups", validate_groups)
