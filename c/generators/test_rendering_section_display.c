// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/rendering_options.h>
#include <SketchUpAPI/model/typed_value.h>
#include <stdbool.h>
#include <stdio.h>

static bool save_bool_option(
    enum SUModelVersion version,
    const char* key,
    bool value,
    const char* path) {
    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SURenderingOptionsRef options = SU_INVALID;
    SUModelGetRenderingOptions(model, &options);
    SUTypedValueRef typed_value = SU_INVALID;
    SUTypedValueCreate(&typed_value);
    SUTypedValueSetBool(typed_value, value);

    bool configured =
        SURenderingOptionsSetValue(options, key, typed_value) == SU_ERROR_NONE;
    bool saved = configured && save_model_with_version(model, path, version);
    if (!configured) fprintf(stderr, "Unsupported rendering option: %s\n", key);

    SUTypedValueRelease(&typed_value);
    SUModelRelease(&model);
    return saved;
}

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion version = version_from_args(argc, argv);

    bool success = true;
    success &= save_bool_option(
        version,
        "DisplaySectionPlanes",
        false,
        "data/rendering_section_planes_false.skp");
    success &= save_bool_option(
        version,
        "DisplaySectionPlanes",
        true,
        "data/rendering_section_planes_true.skp");
    success &= save_bool_option(
        version,
        "DisplaySectionCuts",
        false,
        "data/rendering_section_cuts_false.skp");
    success &= save_bool_option(
        version,
        "DisplaySectionCuts",
        true,
        "data/rendering_section_cuts_true.skp");

    term_sketchup();
    return success ? 0 : 1;
}
