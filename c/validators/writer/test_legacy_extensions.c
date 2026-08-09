// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/attribute_dictionary.h>
#include <SketchUpAPI/model/typed_value.h>

#define PAYLOAD_CAPACITY 8192

static bool validate_legacy_extensions(const char* directory) {
    const char* fixture = "legacy_extensions";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUAttributeDictionaryRef dictionary = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    SUStringRef text = SU_INVALID;
    char payload[PAYLOAD_CAPACITY] = {0};
    size_t dictionaries = 0;

    opened = open_fixture(
        fixture, directory, "legacy_extensions.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(
        fixture,
        SUModelGetNumAttributeDictionaries(model, &dictionaries));
    REQUIRE(
        fixture,
        dictionaries == 1,
        "expected one model compatibility dictionary");
    REQUIRE_SDK(
        fixture,
        SUModelGetAttributeDictionary(
            model, "SkppyLegacyExtensions", &dictionary));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(
        fixture,
        SUAttributeDictionaryGetValue(dictionary, "PayloadV1", &value));
    REQUIRE_SDK(fixture, SUStringCreate(&text));
    REQUIRE_SDK(fixture, SUTypedValueGetString(value, &text));
    REQUIRE(
        fixture,
        copy_utf8(text, payload, sizeof(payload)),
        "could not decode compatibility payload");
    REQUIRE(
        fixture,
        strstr(payload, "\"line_styles\"") != NULL &&
            strstr(payload, "ExtensionDash") != NULL,
        "line-style extension was not preserved");
    REQUIRE(
        fixture,
        strstr(payload, "\"layer_folders\"") != NULL &&
            strstr(payload, "ExtensionFolder") != NULL,
        "layer-folder extension was not preserved");
    REQUIRE(
        fixture,
        strstr(payload, "\"material_pbr\"") != NULL &&
            strstr(payload, "ExtensionSteel") != NULL,
        "PBR extension was not preserved");
    REQUIRE(
        fixture,
        strstr(payload, "\"environment\"") != NULL &&
            strstr(payload, "ExtensionStudio") != NULL,
        "environment extension was not preserved");
    REQUIRE(
        fixture,
        strstr(payload, "\"raw_arcs\"") != NULL &&
            strstr(payload, "AAECAwQFBgcICQoL") != NULL,
        "raw-arc extension was not preserved");
    REQUIRE(
        fixture,
        strstr(payload, "\"sections\"") != NULL &&
            strstr(payload, "ExtensionCut") != NULL &&
            strstr(payload, "\"symbol\":\"EC\"") != NULL,
        "section-plane extension was not preserved");
    REQUIRE(
        fixture,
        strstr(payload, "\"definitions\"") != NULL &&
            strstr(payload, "ExtensionPacked") != NULL &&
            strstr(payload, "UEFDS0VE") != NULL,
        "definition payload extension was not preserved");
    REQUIRE(
        fixture,
        strstr(payload, "\"shadow_edges_cast_shadows\":true") != NULL,
        "shadow extension was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(text)) SUStringRelease(&text);
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("legacy_extensions", validate_legacy_extensions)
