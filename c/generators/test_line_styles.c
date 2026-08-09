// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/line_style.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Line styles are usually managed via styles or specific APIs
    // SUModelGetLineStyles is available in some versions.
    
    save_model_with_version(model, "data/line_styles.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
