// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/axes.h>

static bool validate_axes(const char* directory) {
    const char* fixture = "axes";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUAxesRef axes = SU_INVALID;
    struct SUPoint3D origin = {0};

    opened = open_fixture(fixture, directory, "axes.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetAxes(model, &axes));
    REQUIRE_SDK(fixture, SUAxesGetOrigin(axes, &origin));
    REQUIRE(fixture,
            almost_equal(origin.x, 10.0) && almost_equal(origin.y, 10.0) &&
                almost_equal(origin.z, 10.0),
            "custom axes origin was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("axes", validate_axes)
