// SPDX-License-Identifier: MIT
#include "test_utils.h"

static bool expected_transform(const char* name, const struct SUTransformation* transform) {
    if (strcmp(name, "Translated") == 0)
        return almost_equal(transform->values[12], 10) &&
               almost_equal(transform->values[13], 20) &&
               almost_equal(transform->values[14], 30);
    if (strcmp(name, "NonUniformScale") == 0)
        return almost_equal(transform->values[0], 2) &&
               almost_equal(transform->values[5], 3) &&
               almost_equal(transform->values[10], 4);
    if (strcmp(name, "MirroredRotation") == 0)
        return almost_equal(transform->values[0], 0) &&
               almost_equal(transform->values[1], 1) &&
               almost_equal(transform->values[4], 1) &&
               almost_equal(transform->values[12], -10);
    return false;
}

static bool validate_component_transforms(const char* directory) {
    const char* fixture = "component_transforms";
    SUModelRef model = SU_INVALID;
    bool opened = open_fixture(fixture, directory, "component_transforms.skp", &model);
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUComponentInstanceRef instances[3] = {SU_INVALID, SU_INVALID, SU_INVALID};
    size_t count = 0;
    size_t retrieved = 0;
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumInstances(entities, &count));
    REQUIRE(fixture, count == 3, "expected three transformed instances");
    REQUIRE_SDK(fixture, SUEntitiesGetInstances(entities, 3, instances, &retrieved));
    REQUIRE(fixture, retrieved == 3, "could not retrieve transformed instances");
    for (size_t index = 0; index < retrieved; ++index) {
        char name[NAME_CAPACITY] = {0};
        struct SUTransformation transform = {{0}};
        REQUIRE(fixture, instance_name(instances[index], name), "could not read instance name");
        REQUIRE_SDK(fixture, SUComponentInstanceGetTransform(instances[index], &transform));
        REQUIRE(fixture, expected_transform(name, &transform), "instance transform did not match its name");
    }
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("component_transforms", validate_component_transforms)
