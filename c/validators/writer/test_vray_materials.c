// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/attribute_dictionary.h>
#include <SketchUpAPI/model/typed_value.h>

#define PLUGIN_CAPACITY 4096

static bool read_string_value(
    const char* fixture,
    SUAttributeDictionaryRef dictionary,
    const char* key,
    SUTypedValueRef value,
    SUStringRef text,
    char* output,
    size_t capacity) {
    if (!check_result(
            fixture,
            "SUAttributeDictionaryGetValue",
            SUAttributeDictionaryGetValue(dictionary, key, &value)))
        return false;
    if (!check_result(
            fixture,
            "SUTypedValueGetString",
            SUTypedValueGetString(value, &text)))
        return false;
    return copy_utf8(text, output, capacity);
}

static bool validate_vray_materials(const char* directory) {
    const char* fixture = "vray_materials";
    SUModelRef model = SU_INVALID;
    SUMaterialRef material = SU_INVALID;
    SUAttributeDictionaryRef info = SU_INVALID;
    SUAttributeDictionaryRef plugins = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    SUStringRef text = SU_INVALID;
    bool opened = false;
    bool valid = false;
    int major = 0;
    int minor = 0;
    int build = 0;
    int32_t version = 0;
    char main_plugin[NAME_CAPACITY] = {0};
    char wrapper[PLUGIN_CAPACITY] = {0};
    char brdf[PLUGIN_CAPACITY] = {0};
    char bitmap[PLUGIN_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "vray_materials.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE(fixture, find_material(model, "V-Ray Paint", &material),
            "could not find V-Ray Paint");
    REQUIRE_SDK(
        fixture,
        SUEntityGetAttributeDictionary(
            SUMaterialToEntity(material), "VRayInfo", &info));
    REQUIRE_SDK(
        fixture,
        SUEntityGetAttributeDictionary(
            SUMaterialToEntity(material), "VRayPlugins", &plugins));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(fixture, SUStringCreate(&text));
    REQUIRE(
        fixture,
        read_string_value(
            fixture,
            info,
            "main_plugin",
            value,
            text,
            main_plugin,
            sizeof(main_plugin)),
        "could not decode V-Ray main plugin");
    REQUIRE(fixture, strcmp(main_plugin, "/V-Ray Paint") == 0,
            "unexpected V-Ray main plugin reference");
    REQUIRE_SDK(fixture, SUAttributeDictionaryGetValue(info, "version", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetInt32(value, &version));
    REQUIRE(fixture, version == (major == 17 ? 42003 : 72002),
            "unexpected V-Ray metadata version");
    REQUIRE(
        fixture,
        read_string_value(
            fixture,
            plugins,
            "/V-Ray Paint",
            value,
            text,
            wrapper,
            sizeof(wrapper)),
        "could not decode V-Ray wrapper plugin");
    REQUIRE(fixture, strstr(wrapper, "\"class\":\"MtlSingleBRDF\"") != NULL,
            "wrapper plugin has the wrong class");
    REQUIRE(fixture, strstr(wrapper, "\"brdf\":\"/V-Ray Paint/VRay Mtl\"") != NULL,
            "wrapper plugin does not reference its BRDF");
    REQUIRE(
        fixture,
        read_string_value(
            fixture,
            plugins,
            "/V-Ray Paint/VRay Mtl",
            value,
            text,
            brdf,
            sizeof(brdf)),
        "could not decode V-Ray BRDF plugin");
    REQUIRE(fixture, strstr(brdf, "\"class\":\"BRDFVRayMtl\"") != NULL,
            "BRDF plugin has the wrong class");
    REQUIRE(
        fixture,
        strstr(brdf, "\"diffuse\":\"/V-Ray Paint/VRay Mtl/Bitmap\"") != NULL,
        "BRDF plugin does not reference its base-colour texture");
    REQUIRE(fixture, strstr(brdf, "\"metalness\":\"0.80000000000000004\"") != NULL,
            "BRDF plugin lost metallic");
    REQUIRE(fixture, strstr(brdf, "\"reflect_glossiness\":\"0.25\"") != NULL,
            "BRDF plugin lost roughness");
    REQUIRE(fixture, strstr(brdf, "\"opacity\":\"0.75\"") != NULL,
            "BRDF plugin lost opacity");
    REQUIRE(
        fixture,
        read_string_value(
            fixture,
            plugins,
            "/V-Ray Paint/VRay Mtl/Bitmap/Bitmap",
            value,
            text,
            bitmap,
            sizeof(bitmap)),
        "could not decode V-Ray bitmap plugin");
    REQUIRE(fixture, strstr(bitmap, "\"class\":\"BitmapBuffer\"") != NULL,
            "bitmap plugin has the wrong class");
    REQUIRE(fixture, strstr(bitmap, "\"file\":\"vray-paint.png\"") != NULL,
            "bitmap plugin lost the embedded image basename");
    valid = true;

cleanup:
    if (SUIsValid(text)) SUStringRelease(&text);
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("vray_materials", validate_vray_materials)
