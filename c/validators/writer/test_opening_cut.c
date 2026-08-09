// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_opening_cut(const char* directory) {
    const char* fixture = "opening_cut";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUComponentDefinitionRef definition = SU_INVALID;
    struct SUComponentBehavior behavior = {0};

    opened = open_fixture(fixture, directory, "opening_cut.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture, find_definition(model, "OpeningComponent", &definition),
            "could not find OpeningComponent");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetBehavior(definition, &behavior));
    REQUIRE(fixture, behavior.component_cuts_opening,
            "cuts-opening behavior was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("opening_cut", validate_opening_cut)
