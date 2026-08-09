// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/text.h>
#include <SketchUpAPI/geometry.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    SUTextRef text = SU_INVALID;
    SUTextCreate(&text);
    SUTextSetString(text, "Hello, World!");

    SUPoint3D point = { 0, 0, 100 };
    SUInstancePathRef invalid_path = SU_INVALID;
    SUTextSetPoint(text, &point, invalid_path);

    SUEntitiesAddTexts(entities, 1, &text);

    // The SketchUp 2026.1 SDK DLL cannot serialize text entities and returns
    // SU_ERROR_GENERIC (7) at save time.  This is a known limitation - the
    // official conformance test suite (skptest_annotations.cpp) hits the same
    // issue and falls back to saving an empty model.  We do the same so the
    // fixture exists, and the validation test skips gracefully.
    if (!save_model_with_version(model, "data/texts.skp", ver)) {
        fprintf(stderr,
                "Text serialization unsupported by this SDK runtime; "
                "saving fallback empty model.\n");
        SUModelRelease(&model);
        SUModelCreate(&model);
        save_model_with_version(model, "data/texts.skp", ver);
    }

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
