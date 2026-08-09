// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/section_plane.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    // Create a Section Plane
    SUSectionPlaneRef section = SU_INVALID;
    SUPlane3D plane = { 0, 0, 1, 0 }; // z = 0
    SUSectionPlaneCreate(&section);
    SUSectionPlaneSetPlane(section, &plane);
    SUSectionPlaneSetName(section, "TestSection");

    SUEntitiesAddSectionPlanes(entities, 1, &section);

    save_model_with_version(model, "data/section_planes.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
