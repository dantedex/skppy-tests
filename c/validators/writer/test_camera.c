// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/camera.h>

static bool validate_camera(const char* directory) {
    const char* fixture = "camera";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUCameraRef camera = SU_INVALID;
    struct SUPoint3D eye = {0};
    struct SUPoint3D target = {0};
    struct SUVector3D up = {0};
    SUStringRef description = SU_INVALID;
    double fov = 0.0;
    bool fov_is_height = true;
    int major = 0;
    int minor = 0;
    int build = 0;
    char text[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "camera.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetCamera(model, &camera));
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE_SDK(
        fixture, SUCameraGetOrientation(camera, &eye, &target, &up));
    REQUIRE(
        fixture,
        almost_equal(eye.x, 100.0) && almost_equal(eye.y, 200.0) &&
            almost_equal(eye.z, 300.0),
        "camera eye was not preserved");
    REQUIRE(
        fixture,
        almost_equal(target.x, 10.0) && almost_equal(target.y, 20.0) &&
            almost_equal(target.z, 30.0),
        "camera target was not preserved");
    if (major <= 17) {
        double view_x = target.x - eye.x;
        double view_y = target.y - eye.y;
        double view_z = target.z - eye.z;
        double dot = view_x * up.x + view_y * up.y + view_z * up.z;
        double length = sqrt(up.x * up.x + up.y * up.y + up.z * up.z);
        REQUIRE(
            fixture,
            almost_equal(dot, 0.0) && almost_equal(length, 1.0),
            "legacy camera up vector was not normalized");
    } else {
        REQUIRE(
            fixture,
            almost_equal(up.x, 0.0) && almost_equal(up.y, 0.0) &&
                almost_equal(up.z, 1.0),
            "camera up vector was not preserved");
    }
    REQUIRE_SDK(fixture, SUCameraGetPerspectiveFrustumFOV(camera, &fov));
    REQUIRE(fixture, almost_equal(fov, 42.0), "camera FOV was not preserved");
    REQUIRE_SDK(fixture, SUCameraGetFOVIsHeight(camera, &fov_is_height));
    REQUIRE(fixture, !fov_is_height, "camera FOV direction was not preserved");
    REQUIRE_SDK(fixture, SUStringCreate(&description));
    REQUIRE_SDK(fixture, SUCameraGetDescription(camera, &description));
    REQUIRE(
        fixture,
        copy_utf8(description, text, sizeof(text)),
        "could not decode camera description");
    REQUIRE(
        fixture,
        strcmp(text, "WriterCamera") == 0,
        "camera description was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(description)) SUStringRelease(&description);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("camera", validate_camera)
