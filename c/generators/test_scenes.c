// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/scene.h>
#include <SketchUpAPI/model/camera.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Create a Camera
    SUCameraRef camera = SU_INVALID;
    SUCameraCreate(&camera);
    SUPoint3D eye = { 100, 100, 100 };
    SUPoint3D target = { 0, 0, 0 };
    SUVector3D up = { 0, 0, 1 };
    SUCameraSetOrientation(camera, &eye, &target, &up);

    // Create a Scene
    SUSceneRef scene = SU_INVALID;
    SUSceneCreate(&scene);
    SUSceneSetName(scene, "TestScene");
    SUSceneSetCamera(scene, camera);
    SUModelAddScenes(model, 1, &scene);

    save_model_with_version(model, "data/scenes.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
