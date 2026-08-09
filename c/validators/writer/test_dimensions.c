// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/dimension.h>
#include <SketchUpAPI/model/dimension_linear.h>
#include <SketchUpAPI/model/dimension_radial.h>
#include <SketchUpAPI/model/instancepath.h>
#include <SketchUpAPI/model/font.h>

static bool validate_dimensions(const char* directory) {
    const char* fixture = "dimensions";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUDimensionRef dimensions_found[2] = {SU_INVALID, SU_INVALID};
    SUDimensionRef dimension = SU_INVALID;
    SUDimensionRef radial_dimension = SU_INVALID;
    SUDimensionLinearRef linear = SU_INVALID;
    SUDimensionRadialRef radial = SU_INVALID;
    SUStringRef text = SU_INVALID;
    SUFontRef font = SU_INVALID;
    enum SUDimensionType type = SUDimensionType_Invalid;
    enum SUArrowType arrow = SUArrowNone;
    bool is_diameter = false;
    bool casts_shadows = true;
    bool receives_shadows = true;
    struct SUPoint3D connection = {0.0, 0.0, 0.0};
    SUInstancePathRef start_path = SU_INVALID;
    SUInstancePathRef end_path = SU_INVALID;
    size_t path_depth = 0;
    size_t full_depth = 0;
    char text_value[NAME_CAPACITY] = {0};
    size_t dimensions = 0;
    size_t retrieved = 0;

    opened = open_fixture(fixture, directory, "dimensions.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumDimensions(entities, &dimensions));
    REQUIRE(fixture, dimensions == 2, "expected linear and radial dimensions");
    REQUIRE_SDK(
        fixture,
        SUEntitiesGetDimensions(entities, 2, dimensions_found, &retrieved));
    REQUIRE(fixture, retrieved == 2, "dimensions could not be retrieved");
    for (size_t index = 0; index < retrieved; ++index) {
        REQUIRE_SDK(fixture, SUDimensionGetType(dimensions_found[index], &type));
        if (type == SUDimensionType_Linear) dimension = dimensions_found[index];
        if (type == SUDimensionType_Radial) radial_dimension = dimensions_found[index];
    }
    REQUIRE(fixture, SUIsValid(dimension) && SUIsValid(radial_dimension),
            "both dimension types were not present");
    REQUIRE_SDK(fixture, SUDimensionGetType(dimension, &type));
    REQUIRE(fixture, type == SUDimensionType_Linear,
            "dimension did not load as a linear dimension");
    linear = SUDimensionLinearFromDimension(dimension);
    REQUIRE(fixture, SUIsValid(linear), "linear dimension downcast failed");
    REQUIRE_SDK(fixture, SUStringCreate(&text));
    REQUIRE_SDK(fixture, SUDimensionGetText(dimension, &text));
    REQUIRE(fixture, copy_utf8(text, text_value, sizeof(text_value)),
            "dimension text could not be copied");
    REQUIRE(fixture, strcmp(text_value, "Writer length") == 0,
            "dimension text was not preserved");
    REQUIRE_SDK(fixture, SUDimensionGetArrowType(dimension, &arrow));
    REQUIRE(fixture, arrow == SUArrowClosed,
            "dimension arrow type was not preserved");
    REQUIRE_SDK(
        fixture,
        SUDrawingElementGetCastsShadows(
            SUDimensionToDrawingElement(dimension), &casts_shadows));
    REQUIRE(fixture, !casts_shadows,
            "linear dimension cast-shadows state was not preserved");
    REQUIRE_SDK(
        fixture,
        SUDimensionLinearGetStartPoint(linear, &connection, &start_path));
    REQUIRE_SDK(fixture, SUInstancePathGetPathDepth(start_path, &path_depth));
    REQUIRE(fixture, path_depth == 1,
            "linear start association lost its component path");
    REQUIRE_SDK(fixture, SUInstancePathGetFullDepth(start_path, &full_depth));
    REQUIRE(fixture, full_depth == 2,
            "linear start association lost its leaf entity");
    REQUIRE_SDK(
        fixture,
        SUDimensionLinearGetEndPoint(linear, &connection, &end_path));
    REQUIRE_SDK(fixture, SUInstancePathGetPathDepth(end_path, &path_depth));
    REQUIRE(fixture, path_depth == 0,
            "linear end association unexpectedly gained a component path");
    REQUIRE_SDK(fixture, SUInstancePathGetFullDepth(end_path, &full_depth));
    REQUIRE(fixture, full_depth == 1,
            "linear end association lost its leaf entity");
    radial = SUDimensionRadialFromDimension(radial_dimension);
    REQUIRE(fixture, SUIsValid(radial), "radial dimension downcast failed");
    REQUIRE_SDK(fixture, SUDimensionRadialGetDiameter(radial, &is_diameter));
    REQUIRE(fixture, is_diameter, "radial diameter state was not preserved");
    REQUIRE_SDK(fixture, SUDimensionGetFont(radial_dimension, &font));
    REQUIRE_SDK(fixture, SUFontGetFaceName(font, &text));
    REQUIRE(fixture, copy_utf8(text, text_value, sizeof(text_value)),
            "radial font name could not be copied");
    REQUIRE(fixture, strcmp(text_value, "Courier New") == 0,
            "radial font object reference was not preserved");
    REQUIRE_SDK(
        fixture,
        SUDrawingElementGetReceivesShadows(
            SUDimensionToDrawingElement(radial_dimension), &receives_shadows));
    REQUIRE(fixture, !receives_shadows,
            "radial dimension receive-shadows state was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(start_path)) SUInstancePathRelease(&start_path);
    if (SUIsValid(end_path)) SUInstancePathRelease(&end_path);
    if (SUIsValid(text)) SUStringRelease(&text);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("dimensions", validate_dimensions)
