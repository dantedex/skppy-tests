// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_component_behavior(const char* directory) {
    const char* fixture = "component_behavior";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUComponentDefinitionRef definition = SU_INVALID;
    struct SUComponentBehavior behavior = {0};

    opened = open_fixture(fixture, directory, "component_behavior.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture,
            find_definition(model, "BehaviorComponent", &definition),
            "could not find BehaviorComponent");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetBehavior(definition, &behavior));
    REQUIRE(fixture, behavior.component_always_face_camera,
            "always-face-camera behavior was not preserved");
    REQUIRE(fixture, !behavior.component_cuts_opening,
            "component unexpectedly cuts openings");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("component_behavior", validate_component_behavior)
