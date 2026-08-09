// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/image.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    // Create an Image
    SUImageRef image = SU_INVALID;
    // We assume a resource exists
    if (SUImageCreateFromFile(&image, "resources/brick_wall/brick_wall_21_basecolor.png") == SU_ERROR_NONE) {
        SUEntitiesAddImage(entities, image);
    }

    save_model_with_version(model, "data/image.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
