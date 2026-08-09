// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

bool init_sketchup() {
    SUInitialize();
    return true;
}

void term_sketchup() {
    SUTerminate();
}

bool save_model(SUModelRef model, const char* filename) {
    return save_model_with_version(model, filename, SUModelVersion_Current);
}

bool save_model_with_version(SUModelRef model, const char* filename,
                             enum SUModelVersion version) {
    enum SUResult res = SUModelSaveToFileWithVersion(model, filename, version);
    if (res != SU_ERROR_NONE) {
        fprintf(stderr, "Failed to save model to %s (error code: %d)\n",
                filename, res);
        return false;
    }
    printf("Successfully saved model to %s\n", filename);
    return true;
}

/* Case-insensitive compare of two ASCII strings. */
static int ieq(const char* a, const char* b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        ++a;
        ++b;
    }
    return *a == '\0' && *b == '\0';
}

bool parse_version_arg(const char* token, enum SUModelVersion* out) {
    if (!token || !out) return false;

    /* Accept "current" / "latest". */
    if (ieq(token, "current") || ieq(token, "latest")) {
        *out = SUModelVersion_Current;
        return true;
    }

    /* Skip an optional "SU" / "su" prefix. */
    const char* p = token;
    if ((p[0] == 'S' || p[0] == 's') &&
        (p[1] == 'U' || p[1] == 'u')) {
        p += 2;
    }
    if (*p == '\0') return false;

    /* Bare 4-digit year: 2013..2021. */
    if (strlen(p) == 4 && p[0] == '2') {
        int year = atoi(p);
        switch (year) {
            case 2013: *out = SUModelVersion_SU2013; return true;
            case 2014: *out = SUModelVersion_SU2014; return true;
            case 2015: *out = SUModelVersion_SU2015; return true;
            case 2016: *out = SUModelVersion_SU2016; return true;
            case 2017: *out = SUModelVersion_SU2017; return true;
            case 2018: *out = SUModelVersion_SU2018; return true;
            case 2019: *out = SUModelVersion_SU2019; return true;
            case 2020: *out = SUModelVersion_SU2020; return true;
            case 2021: *out = SUModelVersion_SU2021; return true;
            default: return false;
        }
    }

    /* Single-digit major version: 3..8. */
    if (strlen(p) == 1 && *p >= '3' && *p <= '8') {
        switch (*p - '0') {
            case 3:  *out = SUModelVersion_SU3;  return true;
            case 4:  *out = SUModelVersion_SU4;  return true;
            case 5:  *out = SUModelVersion_SU5;  return true;
            case 6:  *out = SUModelVersion_SU6;  return true;
            case 7:  *out = SUModelVersion_SU7;  return true;
            case 8:  *out = SUModelVersion_SU8;  return true;
            default: return false;
        }
    }
    return false;
}

enum SUModelVersion version_from_args(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        /* "--version=SU8" form. */
        if (strncmp(arg, "--version=", 10) == 0) {
            enum SUModelVersion v;
            if (parse_version_arg(arg + 10, &v))
                return v;
        }
        /* "--version SU8" / legacy Makefile "-version SU8" forms. */
        else if ((ieq(arg, "--version") || ieq(arg, "-version")) &&
                 i + 1 < argc) {
            enum SUModelVersion v;
            if (parse_version_arg(argv[i + 1], &v))
                return v;
            ++i;  /* consume the value even if invalid */
        }
        /* "-v SU8" / "-v=SU8" short form. */
        else if (strncmp(arg, "-v=", 3) == 0) {
            enum SUModelVersion v;
            if (parse_version_arg(arg + 3, &v))
                return v;
        }
        else if (ieq(arg, "-v") && i + 1 < argc) {
            enum SUModelVersion v;
            if (parse_version_arg(argv[i + 1], &v))
                return v;
            ++i;
        }
    }
    return SUModelVersion_Current;
}
