// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool validate_texture_uv(const char* directory) {
    const char* fixture = "texture_uv";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUMaterialRef material = SU_INVALID;
    SUTextureRef texture = SU_INVALID;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    SUVertexRef vertices[2] = {SU_INVALID, SU_INVALID};
    SUUVHelperRef helper = SU_INVALID;
    SUTextureWriterRef texture_writer = SU_INVALID;
    size_t width = 0;
    size_t height = 0;
    size_t retrieved = 0;
    double s_scale = 0.0;
    double t_scale = 0.0;
    struct SUPoint3D points[2] = {{0}};
    struct SUUVQ uvq[2] = {{0}};

    opened = open_fixture(fixture, directory, "texture_uv.skp", &model);
    if (!opened) return false;
    REQUIRE(fixture, find_material(model, "Writer Texture", &material),
            "could not find Writer Texture");
    REQUIRE_SDK(fixture, SUMaterialGetTexture(material, &texture));
    REQUIRE_SDK(
        fixture, SUTextureGetDimensions(texture, &width, &height, &s_scale, &t_scale));
    REQUIRE(fixture, width == 1 && height == 1, "embedded PNG is not 1x1 pixels");
    REQUIRE(fixture, almost_equal(s_scale, 0.5),
            "texture S coordinate factor is not reciprocal of width");
    REQUIRE(fixture, almost_equal(t_scale, 1.0 / 3.0),
            "texture T coordinate factor is not reciprocal of height");

    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve textured face");
    REQUIRE_SDK(fixture, SUFaceGetVertices(face, 2, vertices, &retrieved));
    REQUIRE(fixture, retrieved == 2, "could not retrieve two textured face vertices");
    REQUIRE_SDK(fixture, SUVertexGetPosition(vertices[0], &points[0]));
    REQUIRE_SDK(fixture, SUVertexGetPosition(vertices[1], &points[1]));
    REQUIRE(fixture,
            almost_equal(points[0].x, 0.0) && almost_equal(points[0].y, 0.0) &&
                almost_equal(points[1].x, 2.0) && almost_equal(points[1].y, 0.0),
            "textured face vertex order is not the authored order");
    REQUIRE_SDK(
        fixture, SUFaceGetUVHelper(face, true, false, texture_writer, &helper));
    REQUIRE_SDK(fixture, SUUVHelperGetFrontUVQ(helper, &points[0], &uvq[0]));
    REQUIRE_SDK(fixture, SUUVHelperGetFrontUVQ(helper, &points[1], &uvq[1]));
    REQUIRE(fixture,
            isfinite(uvq[0].u) && isfinite(uvq[0].v) && isfinite(uvq[0].q) &&
                isfinite(uvq[1].u) && isfinite(uvq[1].v) && isfinite(uvq[1].q),
            "UV helper returned non-finite coordinates");
    REQUIRE(fixture,
            almost_equal(uvq[0].u, 0.0) && almost_equal(uvq[0].v, 0.0) &&
                almost_equal(uvq[0].q, 1.0) && almost_equal(uvq[1].u, 1.0) &&
                almost_equal(uvq[1].v, 0.0) && almost_equal(uvq[1].q, 1.0),
            "UV projection does not match the authored identity mapping");
    valid = true;

cleanup:
    if (helper.ptr != NULL) SUUVHelperRelease(&helper);
    if (opened) SUModelRelease(&model);
    return valid;
}


WRITER_VALIDATOR_MAIN("texture_uv", validate_texture_uv)
