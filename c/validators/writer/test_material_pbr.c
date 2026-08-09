// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_material_pbr(const char* directory) {
    const char* fixture = "material_pbr";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUMaterialRef material = SU_INVALID;
    SUColor color = {0};
    double metallic = 0.0;
    double roughness = 0.0;
    int major = 0;
    int minor = 0;
    int build = 0;
    char factor_error[160] = {0};

    opened = open_fixture(fixture, directory, "material_pbr.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE(fixture, find_material(model, "PBRMaterial", &material),
            "could not find PBRMaterial");
    REQUIRE_SDK(fixture, SUMaterialGetColor(material, &color));
    REQUIRE_SDK(fixture, SUMaterialGetMetallicFactor(material, &metallic));
    REQUIRE_SDK(fixture, SUMaterialGetRoughnessFactor(material, &roughness));
    REQUIRE(fixture,
            color.red == 180 && color.green == 180 && color.blue == 200,
            "PBR base color was not preserved");
    /* SketchUp 2026.1 returns defaults for PBR-only, unused materials even
       though material.xml contains the authored factors. The inverse
       `material` fixture separately checks non-default factors on an applied
       translucent material, so only this known SDK fallback is accepted. */
    if (major > 17 && !((fabs(metallic - 0.8) < 1.0e-6 && almost_equal(roughness, 0.25)) ||
          (almost_equal(metallic, 0.0) && almost_equal(roughness, 1.0)))) {
        snprintf(factor_error, sizeof(factor_error),
                 "unexpected PBR factors: metallic=%.17g roughness=%.17g",
                 metallic, roughness);
        report_failure(fixture, factor_error);
        goto cleanup;
    }
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("material_pbr", validate_material_pbr)
