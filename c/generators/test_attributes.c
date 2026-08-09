// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/attribute_dictionary.h>
#include <SketchUpAPI/model/typed_value.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/model/layer.h>
#include <SketchUpAPI/geometry.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    SUPoint3D start = { 0, 0, 0 };
    SUPoint3D end = { 10, 10, 10 };
    SUEdgeRef edge = SU_INVALID;
    SUEdgeCreate(&edge, &start, &end);
    SUEntitiesAddEdges(entities, 1, &edge);

    // Add attribute dictionary to the edge
    SUAttributeDictionaryRef dict = SU_INVALID;
    SUAttributeDictionaryCreate(&dict, "TestData");
    
    SUTypedValueRef value = SU_INVALID;
    SUTypedValueCreate(&value);
    SUTypedValueSetString(value, "Hello SketchUp");
    SUAttributeDictionarySetValue(dict, "Message", value);
    
    SUTypedValueRelease(&value);

    // Attach dictionary to edge
    SUEntityRef entity = SUEdgeToEntity(edge);
    SUEntityAddAttributeDictionary(entity, dict);

    SUVertexRef start_vertex = SU_INVALID;
    SUEdgeGetStartVertex(edge, &start_vertex);
    SUAttributeDictionaryRef vertex_dict = SU_INVALID;
    SUAttributeDictionaryCreate(&vertex_dict, "VertexData");
    SUTypedValueCreate(&value);
    SUTypedValueSetString(value, "start");
    SUAttributeDictionarySetValue(vertex_dict, "Role", value);
    SUTypedValueRelease(&value);
    SUEntityAddAttributeDictionary(SUVertexToEntity(start_vertex), vertex_dict);

    SUMaterialRef material = SU_INVALID;
    SUMaterialCreate(&material);
    SUMaterialSetName(material, "AttributedMaterial");
    SUModelAddMaterials(model, 1, &material);
    SUAttributeDictionaryRef material_dict = SU_INVALID;
    SUAttributeDictionaryCreate(&material_dict, "MaterialData");
    SUTypedValueCreate(&value);
    SUTypedValueSetString(value, "paint");
    SUAttributeDictionarySetValue(material_dict, "Kind", value);
    SUTypedValueRelease(&value);
    SUEntityAddAttributeDictionary(SUMaterialToEntity(material), material_dict);

    SULayerRef layer = SU_INVALID;
    SULayerCreate(&layer);
    SULayerSetName(layer, "AttributedLayer");
    SUModelAddLayers(model, 1, &layer);
    SUAttributeDictionaryRef layer_dict = SU_INVALID;
    SUAttributeDictionaryCreate(&layer_dict, "LayerData");
    SUTypedValueCreate(&value);
    SUTypedValueSetString(value, "architecture");
    SUAttributeDictionarySetValue(layer_dict, "Discipline", value);
    SUTypedValueRelease(&value);
    SUEntityAddAttributeDictionary(SULayerToEntity(layer), layer_dict);

    save_model_with_version(model, "data/attributes.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
