// SPDX-License-Identifier: MIT
#include "test_utils.h"

static size_t face_slot(SUFaceRef face) {
    SUVertexRef vertices[4] = {SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID};
    size_t retrieved = 0;
    double minimum_x = 1.0e100;
    if (SUFaceGetVertices(face, 4, vertices, &retrieved) != SU_ERROR_NONE)
        return 4;
    for (size_t index = 0; index < retrieved; ++index) {
        struct SUPoint3D point = {0};
        if (SUVertexGetPosition(vertices[index], &point) != SU_ERROR_NONE) return 4;
        if (point.x < minimum_x) minimum_x = point.x;
    }
    if (almost_equal(minimum_x, 0.0)) return 0;
    if (almost_equal(minimum_x, 30.0)) return 1;
    if (almost_equal(minimum_x, 60.0)) return 2;
    if (almost_equal(minimum_x, 90.0)) return 3;
    return 4;
}

static bool validate_uv_positions(const char* directory) {
    const char* fixture = "uv_positions";
    const struct SUPoint3D points[4][4] = {
        {{0, 0, 0}, {20, 0, 0}, {20, 20, 0}, {0, 20, 0}},
        {{30, 0, 0}, {50, 0, 0}, {50, 20, 0}, {30, 20, 0}},
        {{60, 0, 0}, {80, 0, 0}, {80, 20, 0}, {60, 20, 0}},
        {{90, 0, 0}, {110, 0, 0}, {110, 0, 20}, {90, 0, 20}},
    };
    const struct SUPoint2D expected[4][4] = {
        {{0, 0}, {2, 0}, {2, 1}, {0, 1}},
        {{0, 1}, {0, 0}, {1, 0}, {1, 1}},
        {{1, 0}, {0, 0}, {0, 1}, {1, 1}},
        {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
    };
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef faces[4] = {SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID};
    SUTextureWriterRef texture_writer = SU_INVALID;
    size_t retrieved = 0;
    size_t count = 0;
    bool seen[4] = {false, false, false, false};

    opened = open_fixture(fixture, directory, "uv_positions.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &count));
    REQUIRE(fixture, count == 4, "expected four positioned UV faces");
    REQUIRE_SDK(fixture, SUEntitiesGetFaces(entities, 4, faces, &retrieved));
    REQUIRE(fixture, retrieved == 4, "could not retrieve all positioned UV faces");
    for (size_t face_index = 0; face_index < 4; ++face_index) {
        size_t slot = face_slot(faces[face_index]);
        SUUVHelperRef helper = SU_INVALID;
        REQUIRE(fixture, slot < 4 && !seen[slot],
                "could not identify a positioned UV face");
        seen[slot] = true;
        REQUIRE_SDK(fixture,
                    SUFaceGetUVHelper(
                        faces[face_index], true, false, texture_writer, &helper));
        for (size_t corner = 0; corner < 4; ++corner) {
            struct SUUVQ uvq = {0};
            REQUIRE_SDK(
                fixture, SUUVHelperGetFrontUVQ(helper, &points[slot][corner], &uvq));
            REQUIRE(fixture, fabs(uvq.q) > 1.0e-12,
                    "UV helper returned a zero projective divisor");
            REQUIRE(fixture,
                    almost_equal(uvq.u / uvq.q, expected[slot][corner].x) &&
                        almost_equal(uvq.v / uvq.q, expected[slot][corner].y),
                    "positioned UV coordinate does not match the Python model");
        }
        SUUVHelperRelease(&helper);
    }
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("uv_positions", validate_uv_positions)
