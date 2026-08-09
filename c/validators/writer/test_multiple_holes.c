// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_multiple_holes(const char* directory) {
    return validate_face_loops(
        "multiple_holes", directory, "multiple_holes.skp", 3);
}


WRITER_VALIDATOR_MAIN("multiple_holes", validate_multiple_holes)
