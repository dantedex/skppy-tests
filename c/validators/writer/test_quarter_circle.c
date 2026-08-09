// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_quarter_circle(const char* directory) {
    return validate_single_arc("quarter_circle", directory, "quarter_circle.skp",
                               6, 10.0, 1.5707963267948966, false);
}

WRITER_VALIDATOR_MAIN("quarter_circle", validate_quarter_circle)
