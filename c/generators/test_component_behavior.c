// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/component_definition.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Create a definition with specific behavior
    SUComponentDefinitionRef def = SU_INVALID;
    SUComponentDefinitionCreate(&def);
    SUComponentDefinitionSetName(def, "BehaviorComponent");
    
    // Set behavior: always face camera
    SUComponentBehavior behavior;
    SUComponentDefinitionGetBehavior(def, &behavior);
    behavior.component_always_face_camera = true;
    SUComponentDefinitionSetBehavior(def, &behavior);
    
    SUModelAddComponentDefinitions(model, 1, &def);

    save_model_with_version(model, "data/component_behavior.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
