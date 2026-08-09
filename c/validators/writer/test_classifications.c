// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/classifications.h>

static bool validate_classifications(const char* directory) {
    const char* fixture = "classifications";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUClassificationsRef classifications = SU_INVALID;
    size_t schemas = 0;

    opened = open_fixture(fixture, directory, "classifications.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetClassifications(model, &classifications));
    REQUIRE(fixture, SUIsValid(classifications),
            "classification manager is invalid");
    REQUIRE_SDK(fixture,
                SUClassificationsGetNumSchemas(classifications, &schemas));
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("classifications", validate_classifications)
