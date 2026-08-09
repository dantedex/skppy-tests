// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_circle_unfilled(const char* directory) {
    return validate_single_arc("circle_unfilled", directory, "circle_unfilled.skp",
                               24, 10.0, 6.283185307179586, true);
}

WRITER_VALIDATOR_MAIN("circle_unfilled", validate_circle_unfilled)
