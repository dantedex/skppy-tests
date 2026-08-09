// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/arccurve.h>
#include <SketchUpAPI/model/curve.h>

static bool validate_arccurve(const char* directory) {
    const char* fixture = "arccurve";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUArcCurveRef arc = SU_INVALID;
    size_t count = 0;
    size_t retrieved = 0;
    size_t edges = 0;
    double radius = 0.0;
    double start_angle = 0.0;
    double end_angle = 0.0;
    struct SUPoint3D center = {0};
    struct SUPoint3D start = {0};
    struct SUPoint3D end = {0};
    struct SUVector3D normal = {0};

    opened = open_fixture(fixture, directory, "arccurve.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumArcCurves(entities, &count));
    REQUIRE(fixture, count == 1, "expected one arc curve");
    REQUIRE_SDK(fixture, SUEntitiesGetArcCurves(entities, 1, &arc, &retrieved));
    REQUIRE(fixture, retrieved == 1, "could not retrieve arc curve");
    REQUIRE_SDK(fixture, SUCurveGetNumEdges(SUArcCurveToCurve(arc), &edges));
    REQUIRE_SDK(fixture, SUArcCurveGetRadius(arc, &radius));
    REQUIRE_SDK(fixture, SUArcCurveGetCenter(arc, &center));
    REQUIRE_SDK(fixture, SUArcCurveGetStartPoint(arc, &start));
    REQUIRE_SDK(fixture, SUArcCurveGetEndPoint(arc, &end));
    REQUIRE_SDK(fixture, SUArcCurveGetNormal(arc, &normal));
    REQUIRE_SDK(fixture, SUArcCurveGetStartAngle(arc, &start_angle));
    REQUIRE_SDK(fixture, SUArcCurveGetEndAngle(arc, &end_angle));
    REQUIRE(fixture, edges == 12, "expected twelve arc segments");
    REQUIRE(fixture, almost_equal(radius, 10.0), "arc radius is not 10");
    REQUIRE(fixture,
            almost_equal(center.x, 0.0) && almost_equal(center.y, 0.0) &&
                almost_equal(center.z, 0.0),
            "arc center was not preserved");
    REQUIRE(fixture,
            almost_equal(start.x, 10.0) && almost_equal(start.y, 0.0) &&
                almost_equal(end.x, 0.0) && almost_equal(end.y, 10.0),
            "arc endpoints were not preserved");
    REQUIRE(fixture,
            almost_equal(normal.x, 0.0) && almost_equal(normal.y, 0.0) &&
                almost_equal(normal.z, 1.0),
            "arc normal was not preserved");
    REQUIRE(fixture, almost_equal(start_angle, 0.0),
            "arc start angle is not zero");
    REQUIRE(fixture, almost_equal(end_angle, 1.5707963267948966),
            "arc end angle is not pi/2");
    valid = true;

cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("arccurve", validate_arccurve)
