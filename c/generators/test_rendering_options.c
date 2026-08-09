// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/rendering_options.h>
#include <SketchUpAPI/model/typed_value.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SURenderingOptionsRef options = SU_INVALID;
    SUModelGetRenderingOptions(model, &options);

    // Change a few rendering options
    SUTypedValueRef value = SU_INVALID;
    SUTypedValueCreate(&value);
    
    // Disable edges
    SUTypedValueSetBool(value, false);
    SURenderingOptionsSetValue(options, "EdgeDisplayMode", value);

    // Change background color (as a string or int? Usually keys are strings)
    // "BackgroundColor" is a common key.
    SUTypedValueSetBool(value, true);
    SURenderingOptionsSetValue(options, "DrawGround", value);

    SUTypedValueRelease(&value);

    save_model_with_version(model, "data/rendering_options.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
