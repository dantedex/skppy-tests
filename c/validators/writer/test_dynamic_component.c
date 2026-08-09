// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/attribute_dictionary.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_dynamic_component(const char* directory) {
    const char* fixture = "dynamic_component";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUComponentDefinitionRef definition = SU_INVALID;
    SUAttributeDictionaryRef dictionary = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    SUStringRef text = SU_INVALID;
    char nominal[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "dynamic_component.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture,
            find_definition(model, "DynamicComponent", &definition),
            "dynamic component definition was not found");
    REQUIRE_SDK(
        fixture,
        SUEntityGetAttributeDictionary(
            SUComponentDefinitionToEntity(definition),
            "dynamic_attributes",
            &dictionary));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(
        fixture,
        SUAttributeDictionaryGetValue(dictionary, "_lenx_nominal", &value));
    REQUIRE_SDK(fixture, SUStringCreate(&text));
    REQUIRE_SDK(fixture, SUTypedValueGetString(value, &text));
    REQUIRE(fixture, copy_utf8(text, nominal, sizeof(nominal)),
            "could not decode dynamic attribute");
    REQUIRE(fixture, strcmp(nominal, "100") == 0,
            "dynamic component attribute was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(text)) SUStringRelease(&text);
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("dynamic_component", validate_dynamic_component)
