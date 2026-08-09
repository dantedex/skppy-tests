// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/scene.h>

static bool validate_background_images(const char* directory) {
    const char* fixture = "background_images";
    SUModelRef model = SU_INVALID;
    SUSceneRef scene = SU_INVALID;
    bool opened = false;
    bool valid = false;
    bool displayed = false;
    size_t count = 0;
    size_t retrieved = 0;

    opened = open_fixture(
        fixture, directory, "background_images.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetNumScenes(model, &count));
    REQUIRE(fixture, count == 1, "expected one match-photo scene");
    REQUIRE_SDK(fixture, SUModelGetScenes(model, 1, &scene, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve match-photo scene");
    REQUIRE_SDK(
        fixture, SUSceneGetPhotoMatchImageDisplayed(scene, &displayed));
    REQUIRE(fixture, displayed, "match-photo image was not displayed");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("background_images", validate_background_images)
