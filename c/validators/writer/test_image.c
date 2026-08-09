// SPDX-License-Identifier: MIT
#include "test_utils.h"

#include <SketchUpAPI/model/image.h>

static bool validate_image(const char* directory) {
    const char* fixture = "image";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUImageRef image = SU_INVALID;
    SUComponentDefinitionRef definition = SU_INVALID;
    size_t images = 0;
    size_t definitions = 0;
    size_t materials = 0;
    size_t retrieved = 0;

    opened = open_fixture(fixture, directory, "image.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumImages(entities, &images));
    REQUIRE_SDK(fixture, SUModelGetNumComponentDefinitions(model, &definitions));
    REQUIRE_SDK(fixture, SUModelGetNumMaterials(model, &materials));
    REQUIRE(fixture, images == 1, "expected one placed image");
    REQUIRE(fixture, definitions == 0,
            "image definition unexpectedly appeared in public definition list");
    REQUIRE(fixture, materials == 1, "expected one image material");
    REQUIRE_SDK(fixture, SUEntitiesGetImages(entities, 1, &image, &retrieved));
    REQUIRE(fixture, retrieved == 1 && SUIsValid(image),
            "could not retrieve placed image");
    REQUIRE_SDK(fixture, SUImageGetDefinition(image, &definition));
    REQUIRE(fixture, SUIsValid(definition),
            "image backing definition could not be retrieved");
    valid = true;
cleanup:
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("image", validate_image)
