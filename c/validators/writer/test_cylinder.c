// SPDX-License-Identifier: MIT
#include "test_utils.h"

static size_t count_unique_vertices(SUEdgeRef* edges, size_t count) {
    SUVertexRef vertices[288] = {{0}};
    size_t unique = 0;
    for (size_t index = 0; index < count; ++index) {
        SUVertexRef endpoints[2] = {SU_INVALID, SU_INVALID};
        if (SUEdgeGetStartVertex(edges[index], &endpoints[0]) != SU_ERROR_NONE ||
            SUEdgeGetEndVertex(edges[index], &endpoints[1]) != SU_ERROR_NONE)
            return 0;
        for (size_t endpoint = 0; endpoint < 2; ++endpoint) {
            bool found = false;
            for (size_t previous = 0; previous < unique; ++previous) {
                if (vertices[previous].ptr == endpoints[endpoint].ptr) {
                    found = true;
                    break;
                }
            }
            if (!found) vertices[unique++] = endpoints[endpoint];
        }
    }
    return unique;
}

static size_t collect_edge_arc_curves(
    SUEdgeRef* edges, size_t count, SUArcCurveRef arcs[4]) {
    size_t unique = 0;
    int32_t entity_ids[4] = {0};
    for (size_t index = 0; index < count; ++index) {
        SUCurveRef curve = SU_INVALID;
        if (SUEdgeGetCurve(edges[index], &curve) != SU_ERROR_NONE) continue;
        SUArcCurveRef arc = SUArcCurveFromCurve(curve);
        if (arc.ptr == NULL) continue;
        int32_t entity_id = 0;
        if (SUEntityGetID(SUCurveToEntity(curve), &entity_id) != SU_ERROR_NONE)
            continue;
        bool found = false;
        for (size_t previous = 0; previous < unique; ++previous) {
            if (entity_ids[previous] == entity_id) {
                found = true;
                break;
            }
        }
        if (!found) {
            if (unique == 4) return 5;
            arcs[unique] = arc;
            entity_ids[unique++] = entity_id;
        }
    }
    return unique;
}

static bool validate_cylinder(const char* directory) {
    const char* fixture = "cylinder";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUEdgeRef* edges = NULL;
    SUFaceRef* faces = NULL;
    SUArcCurveRef arcs[4] = {SU_INVALID, SU_INVALID, SU_INVALID, SU_INVALID};
    size_t edge_count = 0;
    size_t face_count = 0;
    size_t retrieved = 0;
    size_t caps_with_holes = 0;
    size_t collected_arcs = 0;
    char arc_error[192] = {0};

    opened = open_fixture(fixture, directory, "cylinder.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumEdges(entities, false, &edge_count));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &face_count));
    REQUIRE(fixture, edge_count == 144, "expected 144 shared cylinder edges");
    REQUIRE(fixture, face_count == 50, "expected 50 hollow-cylinder faces");
    edges = calloc(edge_count, sizeof(*edges));
    faces = calloc(face_count, sizeof(*faces));
    REQUIRE(fixture, edges != NULL && faces != NULL, "allocation failed");
    REQUIRE_SDK(fixture,
                SUEntitiesGetEdges(entities, false, edge_count, edges, &retrieved));
    REQUIRE(fixture, retrieved == edge_count, "could not retrieve cylinder edges");
    REQUIRE(fixture, count_unique_vertices(edges, edge_count) == 96,
            "expected 96 shared cylinder vertices");
    collected_arcs = collect_edge_arc_curves(edges, edge_count, arcs);
    if (collected_arcs != 4) {
        double radii[2] = {0};
        struct SUPoint3D centers[2] = {{0}};
        for (size_t index = 0; index < collected_arcs && index < 2; ++index) {
            SUArcCurveGetRadius(arcs[index], &radii[index]);
            SUArcCurveGetCenter(arcs[index], &centers[index]);
        }
        snprintf(arc_error, sizeof(arc_error),
                 "expected four arcs, found %u: r/z %.6g/%.6g and %.6g/%.6g",
                 (unsigned)collected_arcs, radii[0], centers[0].z,
                 radii[1], centers[1].z);
        report_failure(fixture, arc_error);
        goto cleanup;
    }
    for (size_t index = 0; index < 4; ++index) {
        size_t curve_edges = 0;
        bool full = false;
        REQUIRE_SDK(
            fixture, SUCurveGetNumEdges(SUArcCurveToCurve(arcs[index]), &curve_edges));
        REQUIRE_SDK(fixture, SUArcCurveGetIsFullCircle(arcs[index], &full));
        REQUIRE(fixture, curve_edges == 24 && full,
                "cylinder seam is not a 24-edge full circle");
    }
    REQUIRE_SDK(fixture, SUEntitiesGetFaces(entities, face_count, faces, &retrieved));
    REQUIRE(fixture, retrieved == face_count, "could not retrieve cylinder faces");
    for (size_t index = 0; index < face_count; ++index) {
        size_t holes = 0;
        REQUIRE_SDK(fixture, SUFaceGetNumInnerLoops(faces[index], &holes));
        if (holes == 1) ++caps_with_holes;
    }
    REQUIRE(fixture, caps_with_holes == 2,
            "expected two annular cylinder caps");
    valid = true;
cleanup:
    free(edges);
    free(faces);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("cylinder", validate_cylinder)
