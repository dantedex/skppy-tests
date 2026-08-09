// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    enum SUResult res = SUModelCreate(&model);
    if (res != SU_ERROR_NONE) {
        fprintf(stderr, "Failed to create model\n");
        term_sketchup();
        return 1;
    }

    save_model_with_version(model, "data/empty.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
