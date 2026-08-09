// SPDX-License-Identifier: MIT
#include "test_utils.h"

#define PATH_CAPACITY 2048
#define EPSILON 1.0e-9

static int failures = 0;

void report_failure(const char* fixture, const char* message) {
    fprintf(stderr, "[writer-validate] FAIL %-20s %s\n", fixture, message);
    ++failures;
}

bool check_result(
    const char* fixture, const char* expression, enum SUResult result) {
    if (result == SU_ERROR_NONE) return true;
    char message[512];
    snprintf(message, sizeof(message), "%s returned SDK error %d", expression, result);
    report_failure(fixture, message);
    return false;
}

bool almost_equal(double left, double right) {
    return fabs(left - right) <= EPSILON;
}

bool copy_utf8(SUStringRef value, char* output, size_t capacity) {
    size_t copied = 0;
    return SUStringGetUTF8(value, capacity, output, &copied) == SU_ERROR_NONE;
}

#define NAME_READER(function_name, reference_type, sdk_function)             \
    bool function_name(reference_type reference, char output[NAME_CAPACITY]) { \
        SUStringRef value = SU_INVALID;                                       \
        if (SUStringCreate(&value) != SU_ERROR_NONE) return false;            \
        bool ok = sdk_function(reference, &value) == SU_ERROR_NONE &&         \
                  copy_utf8(value, output, NAME_CAPACITY);                    \
        SUStringRelease(&value);                                              \
        return ok;                                                            \
    }

NAME_READER(material_name, SUMaterialRef, SUMaterialGetName)
NAME_READER(layer_name, SULayerRef, SULayerGetName)
NAME_READER(folder_name, SULayerFolderRef, SULayerFolderGetName)
NAME_READER(definition_name, SUComponentDefinitionRef, SUComponentDefinitionGetName)
NAME_READER(instance_name, SUComponentInstanceRef, SUComponentInstanceGetName)
NAME_READER(group_name, SUGroupRef, SUGroupGetName)

bool open_fixture(
    const char* fixture,
    const char* directory,
    const char* filename,
    SUModelRef* model) {
    char path[PATH_CAPACITY];
    int written = snprintf(path, sizeof(path), "%s\\%s", directory, filename);
    if (written <= 0 || written >= (int)sizeof(path)) {
        report_failure(fixture, "fixture path is too long");
        return false;
    }
    enum SUModelLoadStatus status = SUModelLoadStatus_Success;
    if (!check_result(
            fixture,
            "SUModelCreateFromFileWithStatus",
            SUModelCreateFromFileWithStatus(model, path, &status))) {
        return false;
    }
    if (status != SUModelLoadStatus_Success) {
        report_failure(fixture, "SDK reported a newer model version");
        SUModelRelease(model);
        return false;
    }
    return true;
}

bool find_material(
    SUModelRef model, const char* expected_name, SUMaterialRef* found) {
    size_t count = 0;
    if (SUModelGetNumMaterials(model, &count) != SU_ERROR_NONE || count == 0)
        return false;
    SUMaterialRef* values = calloc(count, sizeof(*values));
    if (values == NULL) return false;
    size_t retrieved = 0;
    bool matched = false;
    if (SUModelGetMaterials(model, count, values, &retrieved) == SU_ERROR_NONE) {
        for (size_t index = 0; index < retrieved; ++index) {
            char name[NAME_CAPACITY] = {0};
            if (material_name(values[index], name) &&
                strcmp(name, expected_name) == 0) {
                *found = values[index];
                matched = true;
                break;
            }
        }
    }
    free(values);
    return matched;
}

bool find_layer(SUModelRef model, const char* expected_name, SULayerRef* found) {
    size_t count = 0;
    if (SUModelGetNumLayers(model, &count) != SU_ERROR_NONE || count == 0)
        return false;
    SULayerRef* values = calloc(count, sizeof(*values));
    if (values == NULL) return false;
    size_t retrieved = 0;
    bool matched = false;
    if (SUModelGetLayers(model, count, values, &retrieved) == SU_ERROR_NONE) {
        for (size_t index = 0; index < retrieved; ++index) {
            char name[NAME_CAPACITY] = {0};
            if (layer_name(values[index], name) && strcmp(name, expected_name) == 0) {
                *found = values[index];
                matched = true;
                break;
            }
        }
    }
    free(values);
    return matched;
}

bool find_definition(
    SUModelRef model,
    const char* expected_name,
    SUComponentDefinitionRef* found) {
    size_t count = 0;
    if (SUModelGetNumComponentDefinitions(model, &count) != SU_ERROR_NONE ||
        count == 0)
        return false;
    SUComponentDefinitionRef* values = calloc(count, sizeof(*values));
    if (values == NULL) return false;
    size_t retrieved = 0;
    bool matched = false;
    if (SUModelGetComponentDefinitions(model, count, values, &retrieved) ==
        SU_ERROR_NONE) {
        for (size_t index = 0; index < retrieved; ++index) {
            char name[NAME_CAPACITY] = {0};
            if (definition_name(values[index], name) &&
                strcmp(name, expected_name) == 0) {
                *found = values[index];
                matched = true;
                break;
            }
        }
    }
    free(values);
    return matched;
}

bool first_face(SUEntitiesRef entities, SUFaceRef* face) {
    size_t count = 0;
    if (SUEntitiesGetNumFaces(entities, &count) != SU_ERROR_NONE || count == 0)
        return false;
    size_t retrieved = 0;
    return SUEntitiesGetFaces(entities, 1, face, &retrieved) == SU_ERROR_NONE &&
           retrieved == 1;
}

bool validate_face_loops(
    const char* fixture,
    const char* directory,
    const char* filename,
    size_t holes) {
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, filename, &model);
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUFaceRef face = SU_INVALID;
    size_t faces = 0;
    size_t inner_loops = 0;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &faces));
    REQUIRE(fixture, faces == 1, "expected one face");
    REQUIRE(fixture, first_face(entities, &face), "could not retrieve face");
    REQUIRE_SDK(fixture, SUFaceGetNumInnerLoops(face, &inner_loops));
    REQUIRE(fixture, inner_loops == holes, "inner-loop count was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

bool validate_single_arc(
    const char* fixture,
    const char* directory,
    const char* filename,
    size_t segments,
    double radius,
    double end_angle,
    bool full_circle) {
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, filename, &model);
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUArcCurveRef arc = SU_INVALID;
    size_t count = 0;
    size_t retrieved = 0;
    size_t edge_count = 0;
    size_t face_count = 0;
    double actual_radius = 0.0;
    double actual_start = 0.0;
    double actual_end = 0.0;
    bool actual_full = false;
    int major = 0;
    int minor = 0;
    int build = 0;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetVersion(model, &major, &minor, &build));
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumArcCurves(entities, &count));
    REQUIRE(fixture, count == 1, "expected one arc curve");
    REQUIRE_SDK(fixture, SUEntitiesGetNumFaces(entities, &face_count));
    REQUIRE(fixture, face_count == 0, "unfilled arc unexpectedly has a face");
    REQUIRE_SDK(fixture, SUEntitiesGetArcCurves(entities, 1, &arc, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve arc curve");
    REQUIRE_SDK(fixture, SUCurveGetNumEdges(SUArcCurveToCurve(arc), &edge_count));
    REQUIRE_SDK(fixture, SUArcCurveGetRadius(arc, &actual_radius));
    REQUIRE_SDK(fixture, SUArcCurveGetStartAngle(arc, &actual_start));
    REQUIRE_SDK(fixture, SUArcCurveGetEndAngle(arc, &actual_end));
    REQUIRE_SDK(fixture, SUArcCurveGetIsFullCircle(arc, &actual_full));
    REQUIRE(fixture, edge_count == segments, "arc segment count was not preserved");
    REQUIRE(fixture, almost_equal(actual_radius, radius),
            "arc radius was not preserved");
    REQUIRE(fixture, almost_equal(actual_start, 0.0),
            "arc start angle is not zero");
    REQUIRE(fixture,
            almost_equal(actual_end, end_angle) ||
                (major <= 17 && full_circle && almost_equal(actual_end, 0.0)),
            "arc end angle was not preserved");
    REQUIRE(fixture, actual_full == full_circle,
            "full-circle state was not preserved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

int run_writer_validator(
    int argc,
    char** argv,
    const char* fixture,
    bool (*validate)(const char* directory)) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <writer-fixture-directory>\n", argv[0]);
        return 2;
    }
    failures = 0;
    SUInitialize();
    bool valid = validate(argv[1]);
    SUTerminate();
    if (!valid || failures != 0) return 1;
    printf("[writer-validate] PASS %-20s\n", fixture);
    return 0;
}
