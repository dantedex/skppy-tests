// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_billboard(const char* directory) {
    const char* fixture = "billboard";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUComponentDefinitionRef definition = SU_INVALID;
    SUEntitiesRef definition_entities = SU_INVALID;
    SUEntitiesRef root = SU_INVALID;
    struct SUComponentBehavior behavior = {0};
    size_t faces = 0;
    size_t instances = 0;

    opened = open_fixture(fixture, directory, "billboard.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture, find_definition(model, "Billboard", &definition),
            "could not find Billboard definition");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetBehavior(definition, &behavior));
    REQUIRE(fixture, behavior.component_always_face_camera,
            "billboard behavior was not preserved");
    REQUIRE_SDK(fixture,
                SUComponentDefinitionGetEntities(definition, &definition_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(definition_entities, &faces));
    REQUIRE(fixture, faces == 1, "billboard definition does not contain one face");
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &root));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(root, &instances));
    REQUIRE(fixture, instances == 1, "billboard root instance was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("billboard", validate_billboard)
