# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpUvBack(BaseSkpTest):
    def test_uv_back_model(self):
        model = self.load_model("uv_back_mat.skp")

        # 2 materials, 1 face -> 4 edges, 4 vertices
        self.assert_restrictive(model, expected_materials=2, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=4, edges=4, faces=1)

        face = model.entities.faces[0]

        # Front material is stored in ENTITY_MATERIAL_REF (0x07D1).
        self.assertIsNotNone(face.front_material_id)
        # Back material is stored in FACE_EXTRA_FLAG (0x0DAF) when a distinct
        # back material is set via SUGeometryInputFaceSetBackMaterial.
        self.assertIsNotNone(face.back_material_id)

        # The two materials must be distinct.
        self.assertNotEqual(face.front_material_id, face.back_material_id)

        # Per-vertex UVs supplied via SUMaterialInput are not serialized to the
        # 0x2710 texture-projection TLV path by the SketchUp C API, so UV
        # projections are expected to be absent for this fixture.  If a future
        # SketchUp version does emit them, validate the transform shape.
        if face.front_uv is not None:
            self.assertEqual(len(face.front_uv.transform), 9)
        if face.back_uv is not None:
            self.assertEqual(len(face.back_uv.transform), 9)


if __name__ == "__main__":
    unittest.main()
