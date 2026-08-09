// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/shadow_info.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Set Shadow Info / Georeference
    SUShadowInfoRef shadow = SU_INVALID;
    SUModelGetShadowInfo(model, &shadow);
    
    SUTypedValueRef val = SU_INVALID;
    SUTypedValueCreate(&val);
    SUTypedValueSetDouble(val, 45.0);
    SUShadowInfoSetValue(shadow, "Latitude", val);
    
    SUTypedValueSetDouble(val, -120.0);
    SUShadowInfoSetValue(shadow, "Longitude", val);
    
    SUTypedValueRelease(&val);

    save_model_with_version(model, "data/georeference.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
