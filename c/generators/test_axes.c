// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/axes.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Set Custom Axes
    SUAxesRef axes = SU_INVALID;
    SUModelGetAxes(model, &axes);
    
    SUPoint3D origin = { 10, 10, 10 };
    SUVector3D x = { 1, 1, 0 }; // Rotated axes
    SUVector3D y = { -1, 1, 0 };
    SUVector3D z = { 0, 0, 1 };
    SUAxesSetOrigin(axes, &origin);
    SUAxesSetAxesVecs(axes, &x, &y, &z);

    save_model_with_version(model, "data/axes.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
