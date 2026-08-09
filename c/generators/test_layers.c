// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/layer.h>
#include <SketchUpAPI/model/layer_folder.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Create two layers so folder membership exercises a multi-ID payload.
    SULayerRef layers[2] = { SU_INVALID, SU_INVALID };
    SULayerCreate(&layers[0]);
    SULayerSetName(layers[0], "TestLayer");
    SULayerCreate(&layers[1]);
    SULayerSetName(layers[1], "SecondLayer");
    SUModelAddLayers(model, 2, layers);

    // Create a Layer Folder (available in newer SketchUp versions)
    // Note: older SU SDK might not have SULayerFolder. 
    // Assuming we use a version that has it.
    SULayerFolderRef folder = SU_INVALID;
    if (SULayerFolderCreate(&folder, "TestFolder") == SU_ERROR_NONE) {
        SUModelAddLayerFolder(model, folder);
        SULayerFolderAddLayer(folder, layers[0]);
        SULayerFolderAddLayer(folder, layers[1]);
    }

    save_model_with_version(model, "data/layers.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
