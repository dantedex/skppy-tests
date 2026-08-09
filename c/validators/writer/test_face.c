// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_face(const char* directory) {
    return validate_face_loops("face", directory, "face.skp", 0);
}


WRITER_VALIDATOR_MAIN("face", validate_face)
