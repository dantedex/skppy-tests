// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/scene.h>

static bool validate_unicode_names(const char* directory) {
    const char* fixture = "unicode_names";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SULayerRef layer = SU_INVALID;
    SUMaterialRef material = SU_INVALID;
    SUComponentDefinitionRef definition = SU_INVALID;
    SUEntitiesRef entities = SU_INVALID;
    SUComponentInstanceRef instance = SU_INVALID;
    SUSceneRef scene = SU_INVALID;
    SUStringRef value = SU_INVALID;
    size_t instances = 0;
    size_t scenes = 0;
    size_t retrieved = 0;
    char text[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "unicode_names.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture, find_layer(model, "Camada 東京", &layer),
            "Unicode layer name was not preserved");
    REQUIRE(fixture, find_material(model, "Material Café", &material),
            "Unicode material name was not preserved");
    REQUIRE(fixture, find_definition(model, "Componente Ω", &definition),
            "Unicode definition name was not preserved");
    REQUIRE_SDK(fixture, SUStringCreate(&value));
    REQUIRE_SDK(fixture, SUComponentDefinitionGetDescription(definition, &value));
    REQUIRE(fixture, copy_utf8(value, text, sizeof(text)),
            "could not decode definition description");
    REQUIRE(fixture, strcmp(text, "Descrição UTF-8") == 0,
            "Unicode definition description was not preserved");

    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(entities, &instances));
    REQUIRE(fixture, instances == 1, "expected one Unicode-named instance");
    REQUIRE_SDK(fixture,
                SUEntitiesGetInstances(entities, 1, &instance, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve Unicode instance");
    REQUIRE(fixture, instance_name(instance, text),
            "could not decode instance name");
    REQUIRE(fixture, strcmp(text, "Instância ☃") == 0,
            "Unicode instance name was not preserved");

    REQUIRE_SDK(fixture, SUModelGetNumScenes(model, &scenes));
    REQUIRE(fixture, scenes == 1, "expected one Unicode-named scene");
    retrieved = 0;
    REQUIRE_SDK(fixture, SUModelGetScenes(model, 1, &scene, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve Unicode scene");
    REQUIRE_SDK(fixture, SUSceneGetName(scene, &value));
    REQUIRE(fixture, copy_utf8(value, text, sizeof(text)),
            "could not decode scene name");
    REQUIRE(fixture, strcmp(text, "Cena São Paulo") == 0,
            "Unicode scene name was not preserved");
    REQUIRE_SDK(fixture, SUSceneGetDescription(scene, &value));
    REQUIRE(fixture, copy_utf8(value, text, sizeof(text)),
            "could not decode scene description");
    REQUIRE(fixture, strcmp(text, "Visão principal") == 0,
            "Unicode scene description was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(value)) SUStringRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("unicode_names", validate_unicode_names)
