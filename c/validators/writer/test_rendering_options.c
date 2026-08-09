// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/rendering_options.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_rendering_options(const char* directory) {
    const char* fixture = "rendering_options";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SURenderingOptionsRef options = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    int32_t edge_display_mode = -1;
    bool draw_ground = false;

    opened = open_fixture(fixture, directory, "rendering_options.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetRenderingOptions(model, &options));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(fixture,
                SURenderingOptionsGetValue(options, "EdgeDisplayMode", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetInt32(value, &edge_display_mode));
    REQUIRE_SDK(fixture,
                SURenderingOptionsGetValue(options, "DrawGround", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetBool(value, &draw_ground));
    REQUIRE(fixture, edge_display_mode == 0 && draw_ground,
            "rendering options were not preserved");
    valid = true;
cleanup:
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("rendering_options", validate_rendering_options)
