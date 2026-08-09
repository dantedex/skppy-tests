// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/style.h>
#include <SketchUpAPI/model/styles.h>

static bool validate_watermarks(const char* directory) {
    const char* fixture = "watermarks";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUStylesRef styles = SU_INVALID;
    SUStyleRef style = SU_INVALID;
    size_t count = 0;
    size_t retrieved = 0;
    bool displays_watermark = false;
    opened = open_fixture(fixture, directory, "watermarks.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetStyles(model, &styles));
    REQUIRE_SDK(fixture, SUStylesGetNumStyles(styles, &count));
    REQUIRE(fixture, count == 1, "expected one watermark style");
    REQUIRE_SDK(fixture, SUStylesGetStyles(styles, 1, &style, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve watermark style");
    REQUIRE_SDK(
        fixture, SUStyleGetDisplaysWatermark(style, &displays_watermark));
    REQUIRE(
        fixture, displays_watermark, "registered style did not display watermark");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("watermarks", validate_watermarks)
