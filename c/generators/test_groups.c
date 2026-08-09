// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/component_instance.h>
#include <SketchUpAPI/model/group.h>
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

    // Group 1: a simple square face
    {
        SUGroupRef grp = SU_INVALID;
        SUGroupCreate(&grp);
        SUEntitiesAddGroup(root, grp);
        SUEntitiesRef ge = SU_INVALID;
        SUGroupGetEntities(grp, &ge);

        SUPoint3D v[4] = { {0,0,0}, {10,0,0}, {10,10,0}, {0,10,0} };
        SUGeometryInputRef geom = SU_INVALID;
        SUGeometryInputCreate(&geom);
        SUGeometryInputSetVertices(geom, 4, v);
        SULoopInputRef loop = SU_INVALID;
        SULoopInputCreate(&loop);
        for (int i = 0; i < 4; i++) SULoopInputAddVertexIndex(loop, i);
        size_t idx;
        SUGeometryInputAddFace(geom, &loop, &idx);
        SUEntitiesFill(ge, geom, true);
        SUGeometryInputRelease(&geom);
    }

    // Group 2: a nested group containing an edge
    {
        SUGroupRef grp = SU_INVALID;
        SUGroupCreate(&grp);
        SUEntitiesAddGroup(root, grp);
        SUEntitiesRef ge = SU_INVALID;
        SUGroupGetEntities(grp, &ge);

        // Nested group inside group 2
        SUGroupRef inner = SU_INVALID;
        SUGroupCreate(&inner);
        SUEntitiesAddGroup(ge, inner);
        SUEntitiesRef ie = SU_INVALID;
        SUGroupGetEntities(inner, &ie);

        SUPoint3D p1 = {0,0,0}, p2 = {5,5,5};
        SUEdgeRef edge = SU_INVALID;
        SUEdgeCreate(&edge, &p1, &p2);
        SUEntitiesAddEdges(ie, 1, &edge);
    }

    // Group 3: a group with a component instance inside
    {
        SUComponentDefinitionRef def = SU_INVALID;
        SUComponentDefinitionCreate(&def);
        SUComponentDefinitionSetName(def, "GroupedComponent");
        SUModelAddComponentDefinitions(model, 1, &def);
        SUEntitiesRef de = SU_INVALID;
        SUComponentDefinitionGetEntities(def, &de);
        SUPoint3D p1 = {0,0,0}, p2 = {3,0,0};
        SUEdgeRef e = SU_INVALID;
        SUEdgeCreate(&e, &p1, &p2);
        SUEntitiesAddEdges(de, 1, &e);

        SUGroupRef grp = SU_INVALID;
        SUGroupCreate(&grp);
        SUEntitiesAddGroup(root, grp);
        SUEntitiesRef ge = SU_INVALID;
        SUGroupGetEntities(grp, &ge);

        SUComponentInstanceRef inst = SU_INVALID;
        SUComponentDefinitionCreateInstance(def, &inst);
        SUEntitiesAddInstance(ge, inst, NULL);
    }

    save_model_with_version(model, "data/groups.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
