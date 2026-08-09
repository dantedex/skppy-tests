// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/camera.h>
#include <SketchUpAPI/model/scene.h>

static bool validate_scenes(const char* directory) {
    const char* fixture = "scenes";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUSceneRef scene = SU_INVALID;
    SUStringRef name = SU_INVALID;
    SUCameraRef camera = SU_INVALID;
    struct SUPoint3D eye = {0};
    struct SUPoint3D target = {0};
    struct SUVector3D up = {0};
    bool use_camera = false;
    size_t scenes = 0;
    size_t retrieved = 0;
    char text[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "scenes.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetNumScenes(model, &scenes));
    REQUIRE(fixture, scenes == 1, "expected one scene");
    REQUIRE_SDK(fixture, SUModelGetScenes(model, 1, &scene, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve scene");
    REQUIRE_SDK(fixture, SUStringCreate(&name));
    REQUIRE_SDK(fixture, SUSceneGetName(scene, &name));
    REQUIRE(fixture, copy_utf8(name, text, sizeof(text)),
            "could not decode scene name");
    REQUIRE(fixture, strcmp(text, "TestScene") == 0,
            "scene name was not preserved");
    REQUIRE_SDK(fixture, SUSceneGetUseCamera(scene, &use_camera));
    REQUIRE(fixture, use_camera, "scene camera flag was not preserved");
    REQUIRE_SDK(fixture, SUSceneGetCamera(scene, &camera));
    REQUIRE_SDK(fixture, SUCameraGetOrientation(camera, &eye, &target, &up));
    REQUIRE(
        fixture,
        almost_equal(eye.x, 100.0) && almost_equal(eye.y, 100.0) &&
            almost_equal(eye.z, 100.0),
        "scene camera eye was not preserved");
    REQUIRE(
        fixture,
        almost_equal(target.x, 0.0) && almost_equal(target.y, 0.0) &&
            almost_equal(target.z, 0.0),
        "scene camera target was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(name)) SUStringRelease(&name);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("scenes", validate_scenes)
