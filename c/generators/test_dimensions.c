// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/dimension.h>
#include <SketchUpAPI/model/dimension_linear.h>
#include <SketchUpAPI/model/dimension_radial.h>
#include <SketchUpAPI/model/arccurve.h>
#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/component_instance.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/instancepath.h>
#include <SketchUpAPI/geometry.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    SUPoint3D start = {1.0, 2.0, 3.0};
    SUPoint3D end = {5.0, 2.0, 3.0};
    SUComponentDefinitionRef reference_definition = SU_INVALID;
    SUComponentDefinitionCreate(&reference_definition);
    SUComponentDefinitionSetName(reference_definition, "Dimension Reference");
    SUModelAddComponentDefinitions(model, 1, &reference_definition);
    SUEntitiesRef reference_entities = SU_INVALID;
    SUComponentDefinitionGetEntities(reference_definition, &reference_entities);
    SUPoint3D local_start = {0.0, 0.0, 0.0};
    SUPoint3D local_end = {4.0, 0.0, 0.0};
    SUEdgeRef nested_edge = SU_INVALID;
    SUEdgeCreate(&nested_edge, &local_start, &local_end);
    SUEntitiesAddEdges(reference_entities, 1, &nested_edge);
    SUComponentInstanceRef reference_instance = SU_INVALID;
    SUComponentDefinitionCreateInstance(reference_definition, &reference_instance);
    SUTransformation reference_transform = {{
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        1.0, 2.0, 3.0, 1.0,
    }};
    SUComponentInstanceSetTransform(reference_instance, &reference_transform);
    SUEntitiesAddInstance(entities, reference_instance, NULL);
    SUEdgeRef reference_edge = SU_INVALID;
    SUEdgeCreate(&reference_edge, &start, &end);
    SUEntitiesAddEdges(entities, 1, &reference_edge);
    SUInstancePathRef start_path = SU_INVALID;
    SUInstancePathRef end_path = SU_INVALID;
    SUInstancePathCreate(&start_path);
    SUInstancePathPushInstance(start_path, reference_instance);
    SUInstancePathSetLeaf(start_path, SUEdgeToEntity(nested_edge));
    SUInstancePathCreate(&end_path);
    SUInstancePathSetLeaf(end_path, SUEdgeToEntity(reference_edge));
    SUDimensionLinearRef linear = SU_INVALID;
    SUDimensionLinearCreate(
        &linear, &start, start_path, &end, end_path, 2.5);
    SUDimensionRef dimension = SUDimensionLinearToDimension(linear);
    SUDimensionSetText(dimension, "Writer length");
    SUDimensionSetText3D(dimension, false);
    SUDimensionSetArrowType(dimension, SUArrowClosed);
    SUDrawingElementSetCastsShadows(
        SUDimensionToDrawingElement(dimension), false);
    SUEntitiesAddDimensions(entities, 1, &dimension);

    SUPoint3D center = {0.0, 0.0, 0.0};
    SUPoint3D arc_start = {10.0, 0.0, 0.0};
    SUPoint3D arc_end = {0.0, 10.0, 0.0};
    SUVector3D normal = {0.0, 0.0, 1.0};
    SUArcCurveRef arc = SU_INVALID;
    SUArcCurveCreate(&arc, &center, &arc_start, &arc_end, &normal, 12);
    SUEntitiesAddArcCurves(entities, 1, &arc);
    SUInstancePathRef arc_path = SU_INVALID;
    SUInstancePathCreate(&arc_path);
    SUInstancePathSetLeaf(arc_path, SUArcCurveToEntity(arc));
    SUDimensionRadialRef radial = SU_INVALID;
    SUDimensionRadialCreate(&radial, arc_path);
    SUDimensionRef radial_dimension = SUDimensionRadialToDimension(radial);
    SUDimensionSetText(radial_dimension, "Writer radius");
    SUDimensionRadialSetDiameter(radial, true);
    SUDrawingElementSetReceivesShadows(
        SUDimensionToDrawingElement(radial_dimension), false);
    SUEntitiesAddDimensions(entities, 1, &radial_dimension);

    save_model_with_version(model, "data/dimensions.skp", ver);

    SUInstancePathRelease(&arc_path);
    SUInstancePathRelease(&start_path);
    SUInstancePathRelease(&end_path);
    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
