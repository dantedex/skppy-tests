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

    // Snaps are custom grips on component definitions.  Create a definition
    // with a snap-to behavior (Vertical) so the fixture exercises the
    // snap-related behavior fields that skppy parses
    // (behavior_snap_mode, behavior_snap_enabled).
    SUComponentDefinitionRef def = SU_INVALID;
    SUComponentDefinitionCreate(&def);
    SUComponentDefinitionSetName(def, "SnappableComponent");

    SUComponentBehavior behavior;
    SUComponentDefinitionGetBehavior(def, &behavior);
    behavior.component_snap = SUSnapToBehavior_Vertical;
    SUComponentDefinitionSetBehavior(def, &behavior);

    SUModelAddComponentDefinitions(model, 1, &def);

    save_model_with_version(model, "data/snaps.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
