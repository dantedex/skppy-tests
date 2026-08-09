// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/environment.h>
#include <SketchUpAPI/model/environments.h>

static bool validate_environment(const char* directory) {
    const char* fixture = "environment";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEnvironmentsRef environments = SU_INVALID;
    SUEnvironmentRef environment[2] = {SU_INVALID, SU_INVALID};
    SUStringRef name = SU_INVALID;
    size_t count = 0;
    size_t retrieved = 0;
    bool skydome = false;
    bool reflections = false;
    int major = 0;
    int minor = 0;
    int build = 0;
    char text[NAME_CAPACITY] = {0};

    opened = open_fixture(fixture, directory, "environment.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE_SDK(fixture, SUModelGetEnvironments(model, &environments));
    REQUIRE_SDK(fixture, SUEnvironmentsGetCount(environments, &count));
    if (major <= 17) {
        REQUIRE(fixture, count == 0, "legacy model unexpectedly exposed environments");
        valid = true;
        goto cleanup;
    }
    REQUIRE(fixture, count == 2, "expected two environments");
    REQUIRE_SDK(
        fixture,
        SUEnvironmentsGetAll(environments, 2, environment, &retrieved));
    REQUIRE(fixture, retrieved == 2, "could not retrieve environments");
    REQUIRE_SDK(fixture, SUStringCreate(&name));
    REQUIRE_SDK(fixture, SUEnvironmentGetName(environment[0], &name));
    REQUIRE(
        fixture,
        copy_utf8(name, text, sizeof(text)),
        "could not decode environment name");
    REQUIRE(
        fixture,
        strcmp(text, "StudioEnvironment") == 0,
        "environment name was not preserved");
    REQUIRE_SDK(
        fixture, SUEnvironmentGetUseAsSkydome(environment[0], &skydome));
    REQUIRE(fixture, skydome, "skydome flag was not preserved");
    REQUIRE_SDK(
        fixture,
        SUEnvironmentGetUseForReflections(environment[0], &reflections));
    REQUIRE(fixture, reflections, "reflection flag was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(name)) SUStringRelease(&name);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("environment", validate_environment)
