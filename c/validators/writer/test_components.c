// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_components(const char* directory) {
    const char* fixture = "components";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef root = SU_INVALID;
    SUEntitiesRef leaf_entities = SU_INVALID;
    SUEntitiesRef parent_entities = SU_INVALID;
    SUComponentDefinitionRef leaf = SU_INVALID;
    SUComponentDefinitionRef parent = SU_INVALID;
    SUComponentInstanceRef instance = SU_INVALID;
    SUComponentInstanceRef nested_instance = SU_INVALID;
    SUGroupRef group = SU_INVALID;
    SULayerRef entity_layer = SU_INVALID;
    struct SUTransformation transform = {{0}};
    size_t component_definitions = 0;
    size_t group_definitions = 0;
    size_t instances = 0;
    size_t groups = 0;
    size_t faces = 0;
    size_t nested_instances = 0;
    size_t retrieved = 0;
    char name[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "components.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(
        fixture, SUModelGetNumComponentDefinitions(model, &component_definitions));
    REQUIRE_SDK(fixture, SUModelGetNumGroupDefinitions(model, &group_definitions));
    REQUIRE(fixture, component_definitions == 2, "expected two component definitions");
    REQUIRE(fixture, group_definitions == 1, "expected one group definition");
    REQUIRE(fixture, find_definition(model, "Writer Leaf", &leaf),
            "could not find Writer Leaf definition");
    REQUIRE(fixture, find_definition(model, "Writer Parent", &parent),
            "could not find Writer Parent definition");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetEntities(leaf, &leaf_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(leaf_entities, &faces));
    REQUIRE(fixture, faces == 1, "Writer Leaf should contain one face");
    REQUIRE_SDK(fixture, SUComponentDefinitionGetEntities(parent, &parent_entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(parent_entities, &nested_instances));
    REQUIRE(fixture, nested_instances == 1, "Writer Parent should contain one instance");
    REQUIRE_SDK(
        fixture,
        SUEntitiesGetInstances(parent_entities, 1, &nested_instance, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve nested component instance");
    REQUIRE_SDK(
        fixture,
        SUDrawingElementGetLayer(
            SUComponentInstanceToDrawingElement(nested_instance), &entity_layer));
    REQUIRE(fixture, layer_name(entity_layer, name),
            "could not read nested instance layer name");
    REQUIRE(fixture, strcmp(name, "Writer Objects") == 0,
            "nested instance is not owned by Writer Objects");

    REQUIRE_SDK(fixture, SUModelGetEntities(model, &root));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(root, &instances));
    REQUIRE_SDK(fixture, SUEntitiesGetNumGroups(root, &groups));
    REQUIRE(fixture, instances == 1, "expected one root component instance");
    REQUIRE(fixture, groups == 1, "expected one root group");
    retrieved = 0;
    REQUIRE_SDK(fixture, SUEntitiesGetInstances(root, 1, &instance, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve root component instance");
    REQUIRE(fixture, instance_name(instance, name), "could not read instance name");
    REQUIRE(fixture, strcmp(name, "Writer Root Parent") == 0,
            "root instance name was not preserved");
    REQUIRE_SDK(
        fixture,
        SUDrawingElementGetLayer(
            SUComponentInstanceToDrawingElement(instance), &entity_layer));
    REQUIRE(fixture, layer_name(entity_layer, name),
            "could not read root instance layer name");
    REQUIRE(fixture, strcmp(name, "Writer Objects") == 0,
            "root instance is not owned by Writer Objects");
    REQUIRE_SDK(fixture, SUComponentInstanceGetTransform(instance, &transform));
    REQUIRE(fixture,
            almost_equal(transform.values[12], 1.0) &&
                almost_equal(transform.values[13], 2.0) &&
                almost_equal(transform.values[14], 3.0),
            "root instance translation was not preserved");

    retrieved = 0;
    REQUIRE_SDK(fixture, SUEntitiesGetGroups(root, 1, &group, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve root group");
    REQUIRE(fixture, group_name(group, name), "could not read group name");
    REQUIRE(fixture, strcmp(name, "Writer Group") == 0,
            "root group name was not preserved");
    REQUIRE_SDK(
        fixture,
        SUDrawingElementGetLayer(SUGroupToDrawingElement(group), &entity_layer));
    REQUIRE(fixture, layer_name(entity_layer, name),
            "could not read root group layer name");
    REQUIRE(fixture, strcmp(name, "Writer Objects") == 0,
            "root group is not owned by Writer Objects");
    REQUIRE_SDK(fixture, SUGroupGetTransform(group, &transform));
    REQUIRE(fixture,
            almost_equal(transform.values[12], 4.0) &&
                almost_equal(transform.values[13], 5.0) &&
                almost_equal(transform.values[14], 6.0),
            "root group translation was not preserved");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("components", validate_components)
