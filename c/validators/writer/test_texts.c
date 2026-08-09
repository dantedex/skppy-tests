// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/text.h>

static bool validate_texts(const char* directory) {
    const char* fixture = "texts";
    SUModelRef model = SU_INVALID;
    bool opened = false;
    bool valid = false;
    SUEntitiesRef entities = SU_INVALID;
    SUTextRef text = SU_INVALID;
    SUStringRef value = SU_INVALID;
    char text_value[NAME_CAPACITY] = {0};
    size_t texts = 0;
    size_t retrieved = 0;

    opened = open_fixture(fixture, directory, "texts.skp", &model);
    if (!opened) return false;
    REQUIRE_SDK(fixture, SUModelGetEntities(model, &entities));
    REQUIRE_SDK(fixture, SUEntitiesGetNumTexts(entities, &texts));
    REQUIRE(fixture, texts == 1, "expected one text annotation");
    REQUIRE_SDK(fixture, SUEntitiesGetTexts(entities, 1, &text, &retrieved));
    REQUIRE(fixture, retrieved == 1, "text annotation could not be retrieved");
    REQUIRE_SDK(fixture, SUStringCreate(&value));
    REQUIRE_SDK(fixture, SUTextGetString(text, &value));
    REQUIRE(fixture, copy_utf8(value, text_value, sizeof(text_value)),
            "text annotation string could not be copied");
    REQUIRE(fixture, strcmp(text_value, "Writer text") == 0,
            "text annotation string was not preserved");
    valid = true;
cleanup:
    if (SUIsValid(value)) SUStringRelease(&value);
    if (opened) SUModelRelease(&model);
    return valid;
}

WRITER_VALIDATOR_MAIN("texts", validate_texts)
