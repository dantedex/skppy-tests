// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/component_instance.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/model.h>

static int add_instance(
    SUEntitiesRef entities,
    SUComponentDefinitionRef definition,
    const char* name,
    const SUTransformation* transform) {
    SUComponentInstanceRef instance = SU_INVALID;
    if (SUComponentDefinitionCreateInstance(definition, &instance) != SU_ERROR_NONE) {
        return 0;
    }
    SUComponentInstanceSetName(instance, name);
    SUComponentInstanceSetTransform(instance, transform);
    return SUEntitiesAddInstance(entities, instance, NULL) == SU_ERROR_NONE;
}

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion version = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUComponentDefinitionRef definition = SU_INVALID;
    SUComponentDefinitionCreate(&definition);
    SUComponentDefinitionSetName(definition, "TransformProbe");
    SUModelAddComponentDefinitions(model, 1, &definition);

    SUEntitiesRef definition_entities = SU_INVALID;
    SUComponentDefinitionGetEntities(definition, &definition_entities);
    const SUPoint3D start = {0, 0, 0};
    const SUPoint3D end = {1, 0, 0};
    SUEdgeRef edge = SU_INVALID;
    SUEdgeCreate(&edge, &start, &end);
    SUEntitiesAddEdges(definition_entities, 1, &edge);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);
    const SUTransformation translated = {{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        10, 20, 30, 1,
    }};
    const SUTransformation scaled = {{
        2, 0, 0, 0,
        0, 3, 0, 0,
        0, 0, 4, 0,
        -4, 5, 6, 1,
    }};
    /* Swapping X and Y has a negative determinant, so this also covers a mirror. */
    const SUTransformation mirrored_rotated = {{
        0, 1, 0, 0,
        1, 0, 0, 0,
        0, 0, 1, 0,
        -10, -20, -30, 1,
    }};

    if (!add_instance(entities, definition, "Translated", &translated) ||
        !add_instance(entities, definition, "NonUniformScale", &scaled) ||
        !add_instance(entities, definition, "MirroredRotation", &mirrored_rotated)) {
        return 1;
    }

    bool saved = save_model_with_version(
        model, "data/component_transforms.skp", version);
    SUModelRelease(&model);
    term_sketchup();
    return saved ? 0 : 1;
}
