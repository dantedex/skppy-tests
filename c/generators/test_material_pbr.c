// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/color.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Create a PBR material with explicit metallic / roughness factors.
    SUMaterialRef material = SU_INVALID;
    SUMaterialCreate(&material);
    SUMaterialSetName(material, "PBRMaterial");

    SUColor base = { 180, 180, 200, 255 };
    SUMaterialSetColor(material, &base);

    // Enable and set PBR metallic / roughness factors.
    SUMaterialSetMetalnessEnabled(material, true);
    SUMaterialSetMetallicFactor(material, 0.8);

    SUMaterialSetRoughnessEnabled(material, true);
    SUMaterialSetRoughnessFactor(material, 0.25);

    SUModelAddMaterials(model, 1, &material);

    save_model_with_version(model, "data/material_pbr.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
