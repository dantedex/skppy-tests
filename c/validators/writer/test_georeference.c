// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/shadow_info.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_georeference(const char* directory) {
    const char* fixture = "georeference";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUShadowInfoRef shadow = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    double latitude = 0.0;
    double longitude = 0.0;

    opened = open_fixture(fixture, directory, "georeference.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetShadowInfo(model, &shadow));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(fixture, SUShadowInfoGetValue(shadow, "Latitude", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetDouble(value, &latitude));
    REQUIRE_SDK(fixture, SUShadowInfoGetValue(shadow, "Longitude", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetDouble(value, &longitude));
    REQUIRE(fixture,
            almost_equal(latitude, 45.0) && almost_equal(longitude, -120.0),
            "latitude or longitude was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("georeference", validate_georeference)
