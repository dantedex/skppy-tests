// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/classifications.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Classifications API
    SUClassificationsRef classifications = SU_INVALID;
    SUModelGetClassifications(model, &classifications);

    save_model_with_version(model, "data/classifications.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
