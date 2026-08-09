// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/component_instance.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/geometry_input.h>
#include <SketchUpAPI/geometry.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef root = SU_INVALID;
    SUModelGetEntities(model, &root);

    // A billboard: a component definition containing a single face, with
    // the "always face camera" behavior enabled, and an instance placed in
    // the model.
    SUComponentDefinitionRef def = SU_INVALID;
    SUComponentDefinitionCreate(&def);
    SUComponentDefinitionSetName(def, "Billboard");

    SUComponentBehavior behavior;
    SUComponentDefinitionGetBehavior(def, &behavior);
    behavior.component_always_face_camera = true;
    SUComponentDefinitionSetBehavior(def, &behavior);

    SUModelAddComponentDefinitions(model, 1, &def);

    // Add a face to the definition
    SUEntitiesRef def_entities = SU_INVALID;
    SUComponentDefinitionGetEntities(def, &def_entities);

    SUPoint3D v[4] = { {0,0,0}, {10,0,0}, {10,10,0}, {0,10,0} };
    SUGeometryInputRef geom = SU_INVALID;
    SUGeometryInputCreate(&geom);
    SUGeometryInputSetVertices(geom, 4, v);
    SULoopInputRef loop = SU_INVALID;
    SULoopInputCreate(&loop);
    for (int i = 0; i < 4; i++) SULoopInputAddVertexIndex(loop, i);
    size_t idx;
    SUGeometryInputAddFace(geom, &loop, &idx);
    SUEntitiesFill(def_entities, geom, true);
    SUGeometryInputRelease(&geom);

    // Place an instance
    SUComponentInstanceRef inst = SU_INVALID;
    SUComponentDefinitionCreateInstance(def, &inst);
    SUEntitiesAddInstance(root, inst, NULL);

    save_model_with_version(model, "data/billboard.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
