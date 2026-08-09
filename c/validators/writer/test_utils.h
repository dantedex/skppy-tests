// SPDX-License-Identifier: MIT
#ifndef SKPPY_WRITER_TEST_UTILS_H_
#define SKPPY_WRITER_TEST_UTILS_H_

#include <SketchUpAPI/color.h>
#include <SketchUpAPI/geometry.h>
#include <SketchUpAPI/initialize.h>
#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/component_instance.h>
#include <SketchUpAPI/model/arccurve.h>
#include <SketchUpAPI/model/curve.h>
#include <SketchUpAPI/model/drawing_element.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/entity.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/group.h>
#include <SketchUpAPI/model/guide_line.h>
#include <SketchUpAPI/model/guide_point.h>
#include <SketchUpAPI/model/layer.h>
#include <SketchUpAPI/model/layer_folder.h>
#include <SketchUpAPI/model/loop.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/section_plane.h>
#include <SketchUpAPI/model/texture.h>
#include <SketchUpAPI/model/uv_helper.h>
#include <SketchUpAPI/model/vertex.h>
#include <SketchUpAPI/sketchup.h>
#include <SketchUpAPI/unicodestring.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_CAPACITY 256

void report_failure(const char* fixture, const char* message);
bool check_result(const char* fixture, const char* expression, enum SUResult result);
bool almost_equal(double left, double right);
bool copy_utf8(SUStringRef value, char* output, size_t capacity);
bool material_name(SUMaterialRef material, char output[NAME_CAPACITY]);
bool layer_name(SULayerRef layer, char output[NAME_CAPACITY]);
bool folder_name(SULayerFolderRef folder, char output[NAME_CAPACITY]);
bool definition_name(
    SUComponentDefinitionRef definition, char output[NAME_CAPACITY]);
bool instance_name(SUComponentInstanceRef instance, char output[NAME_CAPACITY]);
bool group_name(SUGroupRef group, char output[NAME_CAPACITY]);
bool open_fixture(
    const char* fixture,
    const char* directory,
    const char* filename,
    SUModelRef* model);
bool find_material(
    SUModelRef model, const char* expected_name, SUMaterialRef* found);
bool find_layer(SUModelRef model, const char* expected_name, SULayerRef* found);
bool find_definition(
    SUModelRef model,
    const char* expected_name,
    SUComponentDefinitionRef* found);
bool first_face(SUEntitiesRef entities, SUFaceRef* face);
bool validate_face_loops(
    const char* fixture,
    const char* directory,
    const char* filename,
    size_t holes);
bool validate_single_arc(
    const char* fixture,
    const char* directory,
    const char* filename,
    size_t segments,
    double radius,
    double end_angle,
    bool full_circle);
int run_writer_validator(
    int argc,
    char** argv,
    const char* fixture,
    bool (*validate)(const char* directory));

#define REQUIRE_SDK(fixture, expression)                                      \
    do {                                                                       \
        if (!check_result((fixture), #expression, (expression))) goto cleanup; \
    } while (0)

#define REQUIRE(fixture, condition, message)       \
    do {                                            \
        if (!(condition)) {                         \
            report_failure((fixture), (message));   \
            goto cleanup;                           \
        }                                           \
    } while (0)

#define WRITER_VALIDATOR_MAIN(fixture_name, validate_function)              \
    int main(int argc, char** argv) {                                        \
        return run_writer_validator(                                         \
            argc, argv, (fixture_name), (validate_function));                \
    }

#endif
