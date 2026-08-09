// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/rendering_options.h>
#include <SketchUpAPI/model/scene.h>
#include <SketchUpAPI/model/style.h>
#include <SketchUpAPI/model/typed_value.h>

static bool validate_legacy_scene_snapshots(const char* directory) {
    const char* fixture = "legacy_scene_snapshots";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUSceneRef scene = SU_INVALID;
    SURenderingOptionsRef rendering_options = SU_INVALID;
    SUStyleRef style = SU_INVALID;
    SUStringRef name = SU_INVALID;
    SUTypedValueRef value = SU_INVALID;
    bool use_rendering_options = false;
    bool draw_ground = false;
    size_t retrieved = 0;
    char text[NAME_CAPACITY] = {0};

    opened = open_fixture(
        fixture, directory, "legacy_scene_snapshots.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetScenes(model, 1, &scene, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve legacy scene");
    REQUIRE_SDK(
        fixture,
        SUSceneGetUseRenderingOptions(scene, &use_rendering_options));
    REQUIRE(
        fixture,
        use_rendering_options,
        "scene rendering-options flag was not preserved");
    REQUIRE_SDK(fixture, SUSceneGetRenderingOptions(scene, &rendering_options));
    REQUIRE_SDK(fixture, SUTypedValueCreate(&value));
    REQUIRE_SDK(
        fixture,
        SURenderingOptionsGetValue(rendering_options, "DrawGround", &value));
    REQUIRE_SDK(fixture, SUTypedValueGetBool(value, &draw_ground));
    REQUIRE(
        fixture,
        draw_ground,
        "scene rendering-options snapshot was not preserved");
    REQUIRE_SDK(fixture, SUSceneGetStyle(scene, &style));
    REQUIRE_SDK(fixture, SUStringCreate(&name));
    REQUIRE_SDK(fixture, SUStyleGetName(style, &name));
    REQUIRE(
        fixture,
        copy_utf8(name, text, sizeof(text)),
        "could not decode scene style name");
    REQUIRE(
        fixture,
        strcmp(text, "SceneStyle") == 0,
        "scene style was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(value)) SUTypedValueRelease(&value);
    if (SUIsValid(name)) SUStringRelease(&name);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN(
    "legacy_scene_snapshots", validate_legacy_scene_snapshots)
