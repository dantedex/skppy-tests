// SPDX-License-Identifier: MIT
#include "../writer/test_utils.h"

#include <SketchUpAPI/model/attribute_dictionary.h>
#include <SketchUpAPI/model/camera.h>
#include <SketchUpAPI/model/scene.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_blender_export(const char* directory) {
    const char* fixture = "blender_export";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef root = SU_INVALID;
    SUEntitiesRef definition_entities = SU_INVALID;
    SUComponentDefinitionRef definition = SU_INVALID;
    SUComponentInstanceRef instances[3] = {SU_INVALID, SU_INVALID, SU_INVALID};
    SUComponentInstanceRef collection_member = SU_INVALID;
    SUMaterialRef material = SU_INVALID;
    SUTextureRef texture = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SUVertexRef vertices[2] = {SU_INVALID, SU_INVALID};
    SUEdgeRef shading_edges[7] = {SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID};
    SUUVHelperRef uv_helper = SU_INVALID;
    SUTextureWriterRef texture_writer = SU_INVALID;
    SULayerRef layer = SU_INVALID;
    SUSceneRef scene = SU_INVALID;
    SUAttributeDictionaryRef dictionary = SU_INVALID;
    SUAttributeDictionaryRef vray_info = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    SUStringRef text = SU_INVALID;
    struct SUTransformation transforms[2] = {{{0}}, {{0}}};
    struct SUTransformation collection_transform = {{0}};
    struct SUPoint3D points[2] = {{0}};
    struct SUUVQ uvq[2] = {{0}};
    size_t definitions = 0;
    size_t root_instances = 0;
    size_t faces = 0;
    size_t scenes = 0;
    size_t width = 0;
    size_t height = 0;
    size_t retrieved = 0;
    size_t inner_loops = 0;
    size_t collection_instances = 0;
    size_t shading_edge_count = 0;
    size_t soft_edges = 0;
    size_t smooth_edges = 0;
    double s_scale = 0.0;
    double t_scale = 0.0;
    char name[NAME_CAPACITY] = {0};
    int major = 0;
    int minor = 0;
    int build = 0;
    int32_t vray_version = 0;

    opened = open_fixture(fixture, directory, "blender_export.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE(fixture, major >= 21, "expected a modern ZIP-based model version");
    REQUIRE_SDK(fixture, SUModelGetNumComponentDefinitions(model, &definitions));
    REQUIRE(fixture, definitions == 3,
            "expected shared mesh, collection mesh, and collection definitions");
    REQUIRE(fixture,
            find_definition(model, "Export Shared Mesh", &definition),
            "could not find exported shared mesh definition");
    REQUIRE_SDK(
        fixture, SUComponentDefinitionGetEntities(definition, &definition_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(definition_entities, &faces));
    REQUIRE(fixture, faces == 1, "shared definition should contain one face");

    REQUIRE_SDK(fixture, SUModelGetEntities(model, &root));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(root, &root_instances));
    REQUIRE(fixture, root_instances == 3,
            "expected two mesh instances and one collection instance");
    REQUIRE_SDK(fixture,
                SUEntitiesGetInstances(root, 3, instances, &retrieved));
    REQUIRE(fixture, retrieved == 3, "could not retrieve all root instances");
    REQUIRE_SDK(fixture, SUComponentInstanceGetTransform(instances[0], &transforms[0]));
    REQUIRE_SDK(fixture, SUComponentInstanceGetTransform(instances[1], &transforms[1]));
    REQUIRE(fixture,
            almost_equal(transforms[0].values[12], 10.0) &&
                almost_equal(transforms[0].values[13], 20.0) &&
                almost_equal(transforms[0].values[14], 30.0),
            "first instance translation was not converted to inches");
    REQUIRE(fixture,
            almost_equal(transforms[1].values[12], 50.0) &&
                almost_equal(transforms[1].values[13], 0.0) &&
                almost_equal(transforms[1].values[14], 0.0),
            "second instance translation was not converted to inches");

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
    REQUIRE(fixture, vray_version == 72002,
            "modern Blender export lost V-Ray metadata");
    REQUIRE_SDK(fixture, SUMaterialGetTexture(material, &texture));
    REQUIRE_SDK(
        fixture, SUTextureGetDimensions(texture, &width, &height, &s_scale, &t_scale));
    REQUIRE(fixture, width == 1 && height == 1,
            "exported texture dimensions were not preserved");
    REQUIRE(fixture, almost_equal(s_scale, 0.4) && almost_equal(t_scale, 0.25),
            "exported texture physical scale was not preserved");
    REQUIRE(fixture, first_face(definition_entities, &face),
            "could not retrieve exported textured face");
    REQUIRE_SDK(fixture, SUFaceGetNumInnerLoops(face, &inner_loops));
    REQUIRE(fixture, inner_loops == 1,
            "coplanar ring face did not preserve its hole");
    retrieved = 0;
    REQUIRE_SDK(fixture, SUFaceGetVertices(face, 2, vertices, &retrieved));
    REQUIRE(fixture, retrieved == 2, "could not retrieve textured face vertices");
    REQUIRE_SDK(fixture, SUVertexGetPosition(vertices[0], &points[0]));
    REQUIRE_SDK(fixture, SUVertexGetPosition(vertices[1], &points[1]));
    REQUIRE_SDK(
        fixture, SUFaceGetUVHelper(face, true, false, texture_writer, &uv_helper));
    REQUIRE_SDK(fixture, SUUVHelperGetFrontUVQ(uv_helper, &points[0], &uvq[0]));
    REQUIRE_SDK(fixture, SUUVHelperGetFrontUVQ(uv_helper, &points[1], &uvq[1]));
    REQUIRE(fixture,
            almost_equal(uvq[0].u, 0.0) && almost_equal(uvq[0].v, 0.0) &&
                almost_equal(uvq[1].u, 1.0) && almost_equal(uvq[1].v, 0.0),
            "exported Blender UV coordinates were not preserved");
    REQUIRE(fixture, find_definition(model, "Export Collection Mesh", &definition),
            "could not find the shading test definition");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetEntities(definition, &definition_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(definition_entities, false, &shading_edge_count));
    REQUIRE(fixture, shading_edge_count == 7, "shading test definition lost edges");
    retrieved = 0;
    REQUIRE_SDK(fixture, SUEntitiesGetEdges(definition_entities, false, 7, shading_edges, &retrieved));
    REQUIRE(fixture, retrieved == 7, "could not retrieve all shading test edges");
    for (size_t index = 0; index < retrieved; ++index) {
        bool soft = false;
        bool smooth = false;
        REQUIRE_SDK(fixture, SUEdgeGetSoft(shading_edges[index], &soft));
        REQUIRE_SDK(fixture, SUEdgeGetSmooth(shading_edges[index], &smooth));
        soft_edges += soft ? 1 : 0;
        smooth_edges += smooth ? 1 : 0;
    }
    REQUIRE(fixture, soft_edges == 1 && smooth_edges == 1,
            "Blender smooth, sharp, or boundary edge shading was not preserved");
    REQUIRE(fixture,
            find_definition(model, "Export Instance Source", &definition),
            "could not find collection instance definition");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetEntities(definition, &definition_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(definition_entities, &collection_instances));
    REQUIRE(fixture, collection_instances == 1, "collection definition duplicated its linked mesh");
    retrieved = 0;
    REQUIRE_SDK(fixture, SUEntitiesGetInstances(definition_entities, 1, &collection_member, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve the collection member instance");
    REQUIRE_SDK(fixture, SUComponentInstanceGetTransform(collection_member, &collection_transform));
    REQUIRE(fixture,
            almost_equal(collection_transform.values[12], 10.0) &&
                almost_equal(collection_transform.values[13], -20.0) &&
                almost_equal(collection_transform.values[14], -20.0),
            "collection member hierarchy or instance offset was not preserved");
    REQUIRE(fixture, find_layer(model, "Export Tag", &layer),
            "could not find collection-backed tag");
    REQUIRE_SDK(
        fixture,
        SUEntityGetAttributeDictionary(
            SUComponentInstanceToEntity(instances[0]), "Blender", &dictionary));
    REQUIRE_SDK(fixture, SUAttributeDictionaryGetValue(dictionary, "author", &value));
    REQUIRE_SDK(fixture, SUStringCreate(&text));
    REQUIRE_SDK(fixture, SUTypedValueGetString(value, &text));
    REQUIRE(fixture, copy_utf8(text, name, sizeof(name)),
            "could not decode exported custom property");
    REQUIRE(fixture, strcmp(name, "integration") == 0,
            "custom property value was not preserved");

    REQUIRE_SDK(fixture, SUModelGetNumScenes(model, &scenes));
    REQUIRE(fixture, scenes == 1, "expected one camera-marker scene");
    retrieved = 0;
    REQUIRE_SDK(fixture, SUModelGetScenes(model, 1, &scene, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve camera-marker scene");
    REQUIRE_SDK(fixture, SUSceneGetUseCamera(scene, &valid));
    REQUIRE(fixture, valid, "camera-marker scene does not use its camera");
    valid = true;

cleanup:
    if (uv_helper.ptr != NULL) SUUVHelperRelease(&uv_helper);
    if (SUIsValid(text)) SUStringRelease(&text);
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("blender_export", validate_blender_export)
