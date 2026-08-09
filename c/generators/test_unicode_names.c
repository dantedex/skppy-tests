// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/component_instance.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/layer.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/scene.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion version = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SULayerRef layer = SU_INVALID;
    SULayerCreate(&layer);
    SULayerSetName(layer, "Camada " "\xE6\x9D\xB1\xE4\xBA\xAC");
    SUModelAddLayers(model, 1, &layer);

    SUMaterialRef material = SU_INVALID;
    SUMaterialCreate(&material);
    SUMaterialSetName(material, "Material Caf" "\xC3\xA9");
    const SUColor color = {64, 128, 192, 255};
    SUMaterialSetColor(material, &color);
    SUModelAddMaterials(model, 1, &material);

    SUComponentDefinitionRef definition = SU_INVALID;
    SUComponentDefinitionCreate(&definition);
    SUComponentDefinitionSetName(definition, "Componente " "\xCE\xA9");
    SUComponentDefinitionSetDescription(
        definition, "Descri" "\xC3\xA7\xC3\xA3" "o UTF-8");
    SUModelAddComponentDefinitions(model, 1, &definition);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);
    SUComponentInstanceRef instance = SU_INVALID;
    SUComponentDefinitionCreateInstance(definition, &instance);
    SUComponentInstanceSetName(
        instance, "Inst" "\xC3\xA2" "ncia " "\xE2\x98\x83");
    SUEntitiesAddInstance(entities, instance, NULL);

    SUSceneRef scene = SU_INVALID;
    SUSceneCreate(&scene);
    SUSceneSetName(scene, "Cena S" "\xC3\xA3" "o Paulo");
    SUSceneSetDescription(scene, "Vis" "\xC3\xA3" "o principal");
    SUModelAddScenes(model, 1, &scene);

    bool saved = save_model_with_version(model, "data/unicode_names.skp", version);
    SUModelRelease(&model);
    term_sketchup();
    return saved ? 0 : 1;
}
