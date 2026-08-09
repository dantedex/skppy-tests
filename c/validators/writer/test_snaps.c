// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_snaps(const char* directory) {
    const char* fixture = "snaps";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUComponentDefinitionRef definition = SU_INVALID;
    struct SUComponentBehavior behavior = {0};

    opened = open_fixture(fixture, directory, "snaps.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture,
            find_definition(model, "SnappableComponent", &definition),
            "snappable component definition was not found");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetBehavior(definition, &behavior));
    REQUIRE(fixture,
            behavior.component_snap == SUSnapToBehavior_Vertical,
            "vertical snap behavior was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("snaps", validate_snaps)
