// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/component_instance.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/geometry.h>
#include <SketchUpAPI/transformation.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // 1. Create a definition "SubComponent" with an edge
    SUComponentDefinitionRef sub_def = SU_INVALID;
    SUComponentDefinitionCreate(&sub_def);
    SUComponentDefinitionSetName(sub_def, "SubComponent");
    SUModelAddComponentDefinitions(model, 1, &sub_def);

    SUEntitiesRef sub_entities = SU_INVALID;
    SUComponentDefinitionGetEntities(sub_def, &sub_entities);
    
    SUPoint3D p1 = {0,0,0}, p2 = {5,0,0};
    SUEdgeRef edge1 = SU_INVALID;
    SUEdgeCreate(&edge1, &p1, &p2);
    SUEntitiesAddEdges(sub_entities, 1, &edge1);

    // 2. Create a definition "ParentComponent" containing "SubComponent"
    SUComponentDefinitionRef parent_def = SU_INVALID;
    SUComponentDefinitionCreate(&parent_def);
    SUComponentDefinitionSetName(parent_def, "ParentComponent");
    SUModelAddComponentDefinitions(model, 1, &parent_def);

    SUEntitiesRef parent_entities = SU_INVALID;
    SUComponentDefinitionGetEntities(parent_def, &parent_entities);

    SUComponentInstanceRef sub_inst = SU_INVALID;
    SUComponentDefinitionCreateInstance(sub_def, &sub_inst);
    SUEntitiesAddInstance(parent_entities, sub_inst, NULL);

    // 3. Add "ParentComponent" instance to model
    SUEntitiesRef model_entities = SU_INVALID;
    SUModelGetEntities(model, &model_entities);

    SUComponentInstanceRef parent_inst = SU_INVALID;
    SUComponentDefinitionCreateInstance(parent_def, &parent_inst);
    
    struct SUTransformation transform = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        10, 20, 30, 1
    };
    SUComponentInstanceSetTransform(parent_inst, &transform);
    SUEntitiesAddInstance(model_entities, parent_inst, NULL);

    save_model_with_version(model, "data/nested_components.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
