// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/options_manager.h>
#include <SketchUpAPI/model/options_provider.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_options(const char* directory) {
    const char* fixture = "options";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUOptionsManagerRef manager = SU_INVALID;
    SUOptionsProviderRef provider = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    int32_t unit = 0;
    bool snap = false;
    double length = 0.0;

    opened = open_fixture(fixture, directory, "options.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetOptionsManager(model, &manager));
    REQUIRE_SDK(
        fixture,
        SUOptionsManagerGetOptionsProviderByName(
            manager, "UnitsOptions", &provider));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(fixture, SUOptionsProviderGetValue(provider, "LengthUnit", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetInt32(value, &unit));
    REQUIRE(fixture, unit == 2, "length unit was not preserved");
    REQUIRE_SDK(
        fixture,
        SUOptionsProviderGetValue(provider, "LengthSnapEnabled", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetBool(value, &snap));
    REQUIRE(fixture, snap, "length snap flag was not preserved");
    REQUIRE_SDK(
        fixture,
        SUOptionsProviderGetValue(provider, "LengthSnapLength", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetDouble(value, &length));
    REQUIRE(fixture, almost_equal(length, 0.25),
            "length snap value was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("options", validate_options)
