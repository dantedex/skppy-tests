// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/attribute_dictionary.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_attributes(const char* directory) {
    const char* fixture = "attributes";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUEdgeRef edge = SU_INVALID;
    SUVertexRef start = SU_INVALID;
    SUVertexRef end = SU_INVALID;
    SUAttributeDictionaryRef dictionary = SU_INVALID;
    SUAttributeDictionaryRef model_dictionary = SU_INVALID;
    SUAttributeDictionaryRef layer_dictionary = SU_INVALID;
    SUAttributeDictionaryRef vertex_dictionary = SU_INVALID;
    SULayerRef layer = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    SUStringRef text = SU_INVALID;
    size_t edges = 0;
    size_t retrieved = 0;
    struct SUPoint3D start_position = {0};
    struct SUPoint3D end_position = {0};
    char message[NAME_CAPACITY] = {0};
    size_t model_dictionaries = 0;

    opened = open_fixture(fixture, directory, "attributes.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &edges));
    REQUIRE(fixture, edges == 1, "expected one edge");
    REQUIRE_SDK(fixture,
                SUEntitiesGetEdges(entities, false, 1, &edge, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve attributed edge");
    REQUIRE_SDK(fixture, SUEdgeGetStartVertex(edge, &start));
    REQUIRE_SDK(fixture, SUEdgeGetEndVertex(edge, &end));
    REQUIRE_SDK(fixture, SUVertexGetPosition(start, &start_position));
    REQUIRE_SDK(fixture, SUVertexGetPosition(end, &end_position));
    REQUIRE(fixture,
            almost_equal(start_position.x, 0.0) &&
                almost_equal(end_position.x, 10.0),
            "edge endpoint positions were not preserved");
    REQUIRE_SDK(
        fixture,
        SUEntityGetAttributeDictionary(SUEdgeToEntity(edge), "TestData", &dictionary));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(
        fixture,
        SUAttributeDictionaryGetValue(dictionary, "Message", &value));
    REQUIRE_SDK(fixture, SUStringCreate(&text));
    REQUIRE_SDK(fixture, SUTypedValueGetString(value, &text));
    REQUIRE(fixture, copy_utf8(text, message, sizeof(message)),
            "could not decode attribute string");
    REQUIRE(fixture, strcmp(message, "Hello SketchUp") == 0,
            "edge attribute value was not preserved");
    REQUIRE_SDK(
        fixture,
        SUEntityGetAttributeDictionary(
            SUVertexToEntity(start), "VertexData", &vertex_dictionary));
    REQUIRE_SDK(
        fixture,
        SUAttributeDictionaryGetValue(vertex_dictionary, "Role", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetString(value, &text));
    REQUIRE(fixture, copy_utf8(text, message, sizeof(message)),
            "could not decode vertex attribute");
    REQUIRE(fixture, strcmp(message, "start") == 0,
            "vertex attribute value was not preserved");
    REQUIRE_SDK(
        fixture,
        SUModelGetNumAttributeDictionaries(model, &model_dictionaries));
    REQUIRE(fixture, model_dictionaries == 1,
            "expected one model attribute dictionary");
    REQUIRE_SDK(
        fixture,
        SUModelGetAttributeDictionary(model, "ModelData", &model_dictionary));
    REQUIRE_SDK(
        fixture,
        SUAttributeDictionaryGetValue(model_dictionary, "Author", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetString(value, &text));
    REQUIRE(fixture, copy_utf8(text, message, sizeof(message)),
            "could not decode model attribute string");
    REQUIRE(fixture, strcmp(message, "skppy") == 0,
            "model attribute value was not preserved");
    REQUIRE(fixture, find_layer(model, "AttributedLayer", &layer),
            "could not find attributed layer");
    REQUIRE_SDK(
        fixture,
        SUEntityGetAttributeDictionary(
            SULayerToEntity(layer), "LayerData", &layer_dictionary));
    REQUIRE_SDK(
        fixture,
        SUAttributeDictionaryGetValue(layer_dictionary, "Discipline", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetString(value, &text));
    REQUIRE(fixture, copy_utf8(text, message, sizeof(message)),
            "could not decode layer attribute");
    REQUIRE(fixture, strcmp(message, "architecture") == 0,
            "layer attribute value was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(text)) SUStringRelease(&text);
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("attributes", validate_attributes)
