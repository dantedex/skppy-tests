// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_face_hole(const char* directory) {
    return validate_face_loops("face_hole", directory, "face_hole.skp", 1);
}


WRITER_VALIDATOR_MAIN("face_hole", validate_face_hole)
