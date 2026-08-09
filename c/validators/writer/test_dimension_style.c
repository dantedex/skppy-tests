// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/arrow_type.h>
#include <SketchUpAPI/model/dimension_style.h>

static bool validate_dimension_style(const char* directory) {
    const char* fixture = "dimension_style";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUDimensionStyleRef style = SU_INVALID;
    bool flag = false;
    enum SUArrowType arrow = SUArrowNone;
    size_t value = 0;
    double tolerance = 0.0;
    SUColor color = {0};

    opened = open_fixture(fixture, directory, "dimension_style.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetDimensionStyle(model, &style));
    REQUIRE_SDK(fixture, SUDimensionStyleGet3D(style, &flag));
    REQUIRE(fixture, flag, "3-D text flag was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetArrowType(style, &arrow));
    REQUIRE(fixture, arrow == SUArrowClosed, "arrow type was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetArrowSize(style, &value));
    REQUIRE(fixture, value == 12, "arrow size was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetExtensionLineOffset(style, &value));
    REQUIRE(fixture, value == 5, "extension offset was not preserved");
    REQUIRE_SDK(
        fixture, SUDimensionStyleGetExtensionLineOvershoot(style, &value));
    REQUIRE(fixture, value == 10, "extension overshoot was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetLineWeight(style, &value));
    REQUIRE(fixture, value == 2, "line weight was not preserved");
    REQUIRE_SDK(
        fixture,
        SUDimensionStyleGetHighlightNonAssociativeDimensions(style, &flag));
    REQUIRE(fixture, flag, "highlight flag was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetShowRadialPrefix(style, &flag));
    REQUIRE(fixture, flag, "radial-prefix flag was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetHideOutOfPlane(style, &flag));
    REQUIRE(fixture, flag, "out-of-plane flag was not preserved");
    REQUIRE_SDK(
        fixture, SUDimensionStyleGetHideOutOfPlaneValue(style, &tolerance));
    REQUIRE(fixture, almost_equal(tolerance, 0.6),
            "out-of-plane tolerance was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetHideSmall(style, &flag));
    REQUIRE(fixture, flag, "hide-small flag was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetHideSmallValue(style, &tolerance));
    REQUIRE(fixture, almost_equal(tolerance, 10.0),
            "small-value tolerance was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetColor(style, &color));
    REQUIRE(fixture,
            color.red == 64 && color.green == 64 && color.blue == 64 &&
                color.alpha == 255,
            "dimension color was not preserved");
    REQUIRE_SDK(fixture, SUDimensionStyleGetTextColor(style, &color));
    REQUIRE(fixture,
            color.red == 10 && color.green == 20 && color.blue == 30 &&
                color.alpha == 255,
            "dimension text color was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("dimension_style", validate_dimension_style)
