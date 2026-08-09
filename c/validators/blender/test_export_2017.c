// SPDX-License-Identifier: MIT
#include "../writer/test_utils.h"

#include <SketchUpAPI/model/attribute_dictionary.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_blender_export_2017(const char* directory) {
    const char* fixture = "blender_export_2017";
    SUModelRef model = SU_INVALID;
    SUEntitiesRef root = SU_INVALID;
    SUEntitiesRef definition_entities = SU_INVALID;
    SUComponentDefinitionRef definition = SU_INVALID;
    SUComponentInstanceRef collection_member = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SUEdgeRef shading_edges[7] = {SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID};
    SUMaterialRef material = SU_INVALID;
    SUAttributeDictionaryRef vray_info = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    bool opened = false;
    bool valid = false;
    size_t definitions = 0;
    size_t instances = 0;
    size_t definition_faces = 0;
    size_t inner_loops = 0;
    size_t retrieved = 0;
    size_t shading_edge_count = 0;
    size_t soft_edges = 0;
    size_t smooth_edges = 0;
    struct SUTransformation collection_transform = {{0}};
    int major = 0;
    int minor = 0;
    int build = 0;
    int32_t vray_version = 0;

    opened = open_fixture(fixture, directory, "blender_export_2017.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE(fixture, major == 17 && minor == 0 && build == 1, "expected SketchUp Make 2017 model version");
    REQUIRE(fixture, find_material(model, "Export Material", &material),
            "could not find exported material");
    REQUIRE_SDK(
        fixture,
        SUEntityGetAttributeDictionary(
            SUMaterialToEntity(material), "VRayInfo", &vray_info));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(
        fixture,
        SUAttributeDictionaryGetValue(vray_info, "version", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetInt32(value, &vray_version));
    REQUIRE(fixture, vray_version == 42003,
            "legacy Blender export lost V-Ray metadata");
    REQUIRE_SDK(fixture, SUModelGetNumComponentDefinitions(model, &definitions));
    REQUIRE(fixture, definitions == 3,
            "legacy export lost reusable component definitions");
    REQUIRE(fixture, find_definition(model, "Export Shared Mesh", &definition),
            "could not find legacy shared mesh definition");
    REQUIRE_SDK(
        fixture,
        SUComponentDefinitionGetEntities(definition, &definition_entities));
    REQUIRE_SDK(
        fixture,
        SUEntitiesGetNumFaces(definition_entities, &definition_faces));
    REQUIRE(fixture, definition_faces == 1,
            "legacy shared definition should contain one face");
    REQUIRE(fixture, first_face(definition_entities, &face),
            "could not retrieve legacy textured face");
    REQUIRE_SDK(fixture, SUFaceGetNumInnerLoops(face, &inner_loops));
    REQUIRE(fixture, inner_loops == 1,
            "legacy coplanar ring face did not preserve its hole");
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &root));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(root, &instances));
    REQUIRE(fixture, instances == 3,
            "legacy export lost root component instances");
    REQUIRE(fixture, find_definition(model, "Export Collection Mesh", &definition),
            "could not find the legacy shading test definition");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetEntities(definition, &definition_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(definition_entities, false, &shading_edge_count));
    REQUIRE(fixture, shading_edge_count == 7, "legacy shading test definition lost edges");
    REQUIRE_SDK(fixture, SUEntitiesGetEdges(definition_entities, false, 7, shading_edges, &retrieved));
    REQUIRE(fixture, retrieved == 7, "could not retrieve all legacy shading test edges");
    for (size_t index = 0; index < retrieved; ++index) {
        bool soft = false;
        bool smooth = false;
        REQUIRE_SDK(fixture, SUEdgeGetSoft(shading_edges[index], &soft));
        REQUIRE_SDK(fixture, SUEdgeGetSmooth(shading_edges[index], &smooth));
        soft_edges += soft ? 1 : 0;
        smooth_edges += smooth ? 1 : 0;
    }
    REQUIRE(fixture, soft_edges == 1 && smooth_edges == 1,
            "legacy Blender smooth, sharp, or boundary edge shading was not preserved");
    retrieved = 0;
    REQUIRE(fixture, find_definition(model, "Export Instance Source", &definition),
            "could not find legacy collection instance definition");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetEntities(definition, &definition_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(definition_entities, &instances));
    REQUIRE(fixture, instances == 1, "legacy collection definition duplicated its linked mesh");
    REQUIRE_SDK(fixture, SUEntitiesGetInstances(definition_entities, 1, &collection_member, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve the legacy collection member instance");
    REQUIRE_SDK(fixture, SUComponentInstanceGetTransform(collection_member, &collection_transform));
    REQUIRE(fixture,
            almost_equal(collection_transform.values[12], 10.0) &&
                almost_equal(collection_transform.values[13], -20.0) &&
                almost_equal(collection_transform.values[14], -20.0),
            "legacy collection member hierarchy or instance offset was not preserved");
    valid = true;

cleanup:
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("blender_export_2017", validate_blender_export_2017)
