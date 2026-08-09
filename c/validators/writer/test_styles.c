// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/style.h>
#include <SketchUpAPI/model/styles.h>

static bool validate_styles(const char* directory) {
    const char* fixture = "styles";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUStylesRef styles = SU_INVALID;
    SUStyleRef style = SU_INVALID;
    SUStringRef name = SU_INVALID;
    size_t count = 0;
    size_t retrieved = 0;
    char text[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "styles.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetStyles(model, &styles));
    REQUIRE_SDK(fixture, SUStylesGetNumStyles(styles, &count));
    REQUIRE(fixture, count == 1, "expected one registered style");
    REQUIRE_SDK(fixture, SUStylesGetStyles(styles, 1, &style, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve registered style");
    REQUIRE_SDK(fixture, SUStringCreate(&name));
    REQUIRE_SDK(fixture, SUStyleGetName(style, &name));
    REQUIRE(fixture, copy_utf8(name, text, sizeof(text)),
            "could not decode style name");
    REQUIRE(fixture, strcmp(text, "WriterStyle") == 0,
            "style name was not preserved");
    REQUIRE_SDK(fixture, SUStylesGetActiveStyle(styles, &style));
    valid = true;
cleanup:
    if (SUIsValid(name)) SUStringRelease(&name);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("styles", validate_styles)
