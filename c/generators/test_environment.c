// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/environment.h>
#include <SketchUpAPI/model/environments.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Retrieve the environments container and add an environment built from
    // the studio.exr HDR resource so the fixture exercises a populated
    // environment block.
    SUEnvironmentsRef envs = SU_INVALID;
    SUModelGetEnvironments(model, &envs);

    SUEnvironmentRef env = SU_INVALID;
    enum SUResult res = SUEnvironmentCreateFromFile(
        &env, "resources/studio.exr", "StudioEnvironment");
    if (res == SU_ERROR_NONE) {
        SUEnvironmentSetUseAsSkydome(env, true);
        SUEnvironmentSetUseForReflections(env, true);
        SUEnvironmentsAdd(envs, env);
    } else {
        fprintf(stderr, "SUEnvironmentCreateFromFile failed (error %d)\n", res);
    }

    SUEnvironmentRef second = SU_INVALID;
    res = SUEnvironmentCreateFromFile(
        &second, "resources/studio.exr", "ReflectionEnvironment");
    if (res == SU_ERROR_NONE) {
        SUEnvironmentSetUseAsSkydome(second, false);
        SUEnvironmentSetUseForReflections(second, true);
        SUEnvironmentsAdd(envs, second);
    } else {
        fprintf(stderr, "second SUEnvironmentCreateFromFile failed (error %d)\n", res);
    }

    save_model_with_version(model, "data/environment.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
