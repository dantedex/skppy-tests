// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/rendering_options.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_option(
    const char* directory,
    const char* filename,
    const char* key,
    bool expected) {
    const char* fixture = "rendering_section_display";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SURenderingOptionsRef options = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    bool actual = false;

    opened = open_fixture(fixture, directory, filename, &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetRenderingOptions(model, &options));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(fixture, SURenderingOptionsGetValue(options, key, &value));
    REQUIRE_SDK(fixture, SUTypedValueGetBool(value, &actual));
    REQUIRE(fixture, actual == expected, "section display option mismatch");
    valid = true;
cleanup:
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

static bool validate_rendering_section_display(const char* directory) {
    return validate_option(
               directory,
               "rendering_section_planes_false.skp",
               "DisplaySectionPlanes",
               false) &&
           validate_option(
               directory,
               "rendering_section_planes_true.skp",
               "DisplaySectionPlanes",
               true) &&
           validate_option(
               directory,
               "rendering_section_cuts_false.skp",
               "DisplaySectionCuts",
               false) &&
           validate_option(
               directory,
               "rendering_section_cuts_true.skp",
               "DisplaySectionCuts",
               true);
}

WRITER_VALIDATOR_MAIN(
    "rendering_section_display", validate_rendering_section_display)
