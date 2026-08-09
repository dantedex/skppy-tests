// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/geometry_input.h>
#include <SketchUpAPI/geometry.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    SUEntitiesRef entities = SU_INVALID;
    SUModelGetEntities(model, &entities);

    // A hollow cylinder (tube) with smooth side edges, matching the
    // reference cilynder.skp structure:
    //   - 24 segments per circle
    //   - 4 ArcCurve entities (bottom-outer, top-outer, bottom-inner, top-inner)
    //   - 2 cap faces with inner loops (hollow)
    //   - 24 outer side faces + 24 inner side faces
    //   - 96 vertices, 144 edges, 50 faces
    const int segs = 24;
    const double r_outer = 39.37007874015748;  // 1 meter in inches
    const double r_inner = 19.758832678565383;  // matches reference (~0.5019 m)
    const double h = 39.37007874015748;         // 1 meter in inches

    // Build all geometry with SUGeometryInput so vertices are shared.
    // Vertex layout:
    //   [0..23]      bottom outer  (r_outer, z=0)
    //   [24..47]     top outer     (r_outer, z=h)
    //   [48..71]     bottom inner  (r_inner, z=0)
    //   [72..95]     top inner     (r_inner, z=h)
    SUPoint3D v[segs * 4];
    for (int i = 0; i < segs; i++) {
        double a = 2.0 * 3.14159265358979 * i / segs;
        double co = cos(a), si = sin(a);
        v[i]            = (SUPoint3D){ r_outer * co, r_outer * si, 0.0 };
        v[i + segs]     = (SUPoint3D){ r_outer * co, r_outer * si, h };
        v[i + segs * 2] = (SUPoint3D){ r_inner * co, r_inner * si, 0.0 };
        v[i + segs * 3] = (SUPoint3D){ r_inner * co, r_inner * si, h };
    }

    SUGeometryInputRef geom = SU_INVALID;
    SUGeometryInputCreate(&geom);
    SUGeometryInputSetVertices(geom, segs * 4, v);

    SUPoint3D center_bot = { 0, 0, 0 };
    SUPoint3D center_top = { 0, 0, h };
    SUVector3D normal = { 0, 0, 1 };

    // Add 4 ArcCurves to the geometry input.  Each ArcCurve references
    // a start and end vertex (the same for a full circle) plus center
    // and normal.  The control_edge_index returned can be used to set
    // soft/smooth flags on the arc's edges.
    size_t ac_bo, ac_to, ac_bi, ac_ti;  // curve indices
    size_t ce_bo, ce_to, ce_bi, ce_ti;  // control edge indices

    SUGeometryInputAddArcCurve(geom, 0, 0, &center_bot, &normal, segs, &ac_bo, &ce_bo);
    SUGeometryInputAddArcCurve(geom, segs, segs, &center_top, &normal, segs, &ac_to, &ce_to);
    SUGeometryInputAddArcCurve(geom, segs * 2, segs * 2, &center_bot, &normal, segs, &ac_bi, &ce_bi);
    SUGeometryInputAddArcCurve(geom, segs * 3, segs * 3, &center_top, &normal, segs, &ac_ti, &ce_ti);

    // Set soft+smooth on all 4 arc curve control edges so the circle
    // edges render smooth.
    SUGeometryInputEdgeSetSoft(geom, ce_bo, true);
    SUGeometryInputEdgeSetSmooth(geom, ce_bo, true);
    SUGeometryInputEdgeSetSoft(geom, ce_to, true);
    SUGeometryInputEdgeSetSmooth(geom, ce_to, true);
    SUGeometryInputEdgeSetSoft(geom, ce_bi, true);
    SUGeometryInputEdgeSetSmooth(geom, ce_bi, true);
    SUGeometryInputEdgeSetSoft(geom, ce_ti, true);
    SUGeometryInputEdgeSetSmooth(geom, ce_ti, true);

    // Outer side faces: bo[i] -> bo[i+1] -> to[i+1] -> to[i]
    // Loop edges: 0=bo circle, 1=vertical, 2=to circle, 3=vertical
    for (int i = 0; i < segs; i++) {
        int j = (i + 1) % segs;
        SULoopInputRef loop = SU_INVALID;
        SULoopInputCreate(&loop);
        SULoopInputAddVertexIndex(loop, i);          // bo[i]
        SULoopInputAddVertexIndex(loop, j);          // bo[j]
        SULoopInputAddVertexIndex(loop, j + segs);   // to[j]
        SULoopInputAddVertexIndex(loop, i + segs);   // to[i]
        // Soft+smooth the vertical edges (indices 1 and 3 in the loop).
        SULoopInputEdgeSetSoft(loop, 1, true);
        SULoopInputEdgeSetSmooth(loop, 1, true);
        SULoopInputEdgeSetSoft(loop, 3, true);
        SULoopInputEdgeSetSmooth(loop, 3, true);
        size_t idx;
        SUGeometryInputAddFace(geom, &loop, &idx);
    }

    // Inner side faces (reversed winding): bi[i] -> ti[i] -> ti[i+1] -> bi[i+1]
    // Loop edges: 0=vertical, 1=ti circle, 2=vertical, 3=bi circle
    for (int i = 0; i < segs; i++) {
        int j = (i + 1) % segs;
        SULoopInputRef loop = SU_INVALID;
        SULoopInputCreate(&loop);
        SULoopInputAddVertexIndex(loop, i + segs * 2);   // bi[i]
        SULoopInputAddVertexIndex(loop, i + segs * 3);   // ti[i]
        SULoopInputAddVertexIndex(loop, j + segs * 3);   // ti[j]
        SULoopInputAddVertexIndex(loop, j + segs * 2);   // bi[j]
        // Soft+smooth the vertical edges (indices 0 and 2 in the loop).
        SULoopInputEdgeSetSoft(loop, 0, true);
        SULoopInputEdgeSetSmooth(loop, 0, true);
        SULoopInputEdgeSetSoft(loop, 2, true);
        SULoopInputEdgeSetSmooth(loop, 2, true);
        size_t idx;
        SUGeometryInputAddFace(geom, &loop, &idx);
    }

    // Bottom cap (normal -Z): outer loop = bo (reversed for -Z), inner loop = bi
    {
        SULoopInputRef outer = SU_INVALID;
        SULoopInputCreate(&outer);
        for (int i = segs - 1; i >= 0; i--) SULoopInputAddVertexIndex(outer, i);
        size_t face_idx;
        SUGeometryInputAddFace(geom, &outer, &face_idx);

        SULoopInputRef inner = SU_INVALID;
        SULoopInputCreate(&inner);
        for (int i = 0; i < segs; i++) SULoopInputAddVertexIndex(inner, i + segs * 2);
        SUGeometryInputFaceAddInnerLoop(geom, face_idx, &inner);
    }

    // Top cap (normal +Z): outer loop = to, inner loop = ti (reversed for hole)
    {
        SULoopInputRef outer = SU_INVALID;
        SULoopInputCreate(&outer);
        for (int i = 0; i < segs; i++) SULoopInputAddVertexIndex(outer, i + segs);
        size_t face_idx;
        SUGeometryInputAddFace(geom, &outer, &face_idx);

        SULoopInputRef inner = SU_INVALID;
        SULoopInputCreate(&inner);
        for (int i = segs - 1; i >= 0; i--) SULoopInputAddVertexIndex(inner, i + segs * 3);
        SUGeometryInputFaceAddInnerLoop(geom, face_idx, &inner);
    }

    SUEntitiesFill(entities, geom, true);
    SUGeometryInputRelease(&geom);

    save_model_with_version(model, "data/cylinder.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
