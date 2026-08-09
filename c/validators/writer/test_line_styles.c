// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/line_style.h>
#include <SketchUpAPI/model/line_styles.h>

static bool validate_line_styles(const char* directory) {
    const char* fixture = "line_styles";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SULineStylesRef line_styles = SU_INVALID;
    SULineStyleRef line_style = SU_INVALID;
    size_t count = 0;
    double width = 0.0;
    double multiplier = 0.0;
    SUColor color = {0};
    bool user_created = false;
    int major = 0;
    int minor = 0;
    int build = 0;

    opened = open_fixture(fixture, directory, "line_styles.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE_SDK(fixture, SUModelGetLineStyles(model, &line_styles));
    REQUIRE(fixture, SUIsValid(line_styles), "line-style manager is invalid");
    REQUIRE_SDK(fixture, SULineStylesGetNumLineStyles(line_styles, &count));
    if (major <= 17) {
        REQUIRE(fixture, count == 12, "legacy model has an unexpected built-in line-style count");
        valid = true;
        goto cleanup;
    }
    REQUIRE(fixture, count == 13, "expected built-in and user line styles");
    REQUIRE_SDK(
        fixture,
        SULineStylesGetLineStyleByName(
            line_styles, "Writer Dash", &line_style));
    REQUIRE_SDK(fixture, SULineStyleGetWidthPixels(line_style, &width));
    REQUIRE(fixture, almost_equal(width, 2.5),
            "line-style width was not preserved");
    REQUIRE_SDK(
        fixture,
        SULineStyleGetLengthMultiplier(line_style, &multiplier));
    REQUIRE(fixture, almost_equal(multiplier, 2.0),
            "line-style multiplier was not preserved");
    REQUIRE_SDK(fixture, SULineStyleGetColor(line_style, &color));
    REQUIRE(fixture,
            color.red == 10 && color.green == 20 && color.blue == 30 &&
                color.alpha == 255,
            "line-style color was not preserved");
    REQUIRE_SDK(fixture, SULineStyleIsUserCreated(line_style, &user_created));
    REQUIRE(fixture, user_created, "line style was not marked as user-created");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("line_styles", validate_line_styles)
