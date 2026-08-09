// SPDX-License-Identifier: MIT
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <SketchUpAPI/sketchup.h>
#include <SketchUpAPI/geometry.h>
#include <SketchUpAPI/model/model_version.h>
#include <stdbool.h>

typedef struct SUPoint2D SUPoint2D;
typedef struct SUVector2D SUVector2D;
typedef struct SUPoint3D SUPoint3D;
typedef struct SUVector3D SUVector3D;
typedef struct SUPlane3D SUPlane3D;
typedef struct SUBoundingBox3D SUBoundingBox3D;
typedef struct SUTransformation SUTransformation;
typedef struct SUComponentBehavior SUComponentBehavior;

bool init_sketchup();
void term_sketchup();

/* Save using the SDK's default (current) version. */
bool save_model(SUModelRef model, const char* filename);

/* Save using a specific SketchUp file-format version. */
bool save_model_with_version(SUModelRef model, const char* filename,
                             enum SUModelVersion version);

/*
 * Parse a version token from argv and return the matching SUModelVersion.
 *
 * Accepted tokens (case-insensitive): SU3, SU4, ..., SU8, SU2013, ...,
 * SU2021, "current", "latest".  Also accepts bare numbers like "8" or
 * "2017".
 *
 * Returns true and stores the version in *out on success.
 * Returns false if the token is not recognised (out is left untouched).
 */
bool parse_version_arg(const char* token, enum SUModelVersion* out);

/*
 * Convenience: scan argv for a "--version <token>", "-version <token>",
 * "--version=<token>", or "-v <token>" option and return the resolved
 * version, or SUModelVersion_Current when the option is absent / invalid.
 *
 * Pass argv/argc straight from main().
 */
enum SUModelVersion version_from_args(int argc, char** argv);

#endif /* TEST_UTILS_H */
