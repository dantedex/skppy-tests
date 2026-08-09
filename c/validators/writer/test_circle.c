// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_circle(const char* directory) {
    return validate_single_arc(
        "circle", directory, "circle.skp", 24, 10.0, 6.283185307179586, true);
}

WRITER_VALIDATOR_MAIN("circle", validate_circle)
