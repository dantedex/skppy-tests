# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpUvPositions(BaseSkpTest):
    def test_uv_position_variants(self):
        model = self.load_model("uv_positions.skp")

        self.assert_restrictive(model, expected_materials=1, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=16, edges=16, faces=4)

        material = model.materials[0]
        self.assertEqual(material.name, "UVConformanceMaterial")
        self.assertIsNotNone(material.texture)

        for face in model.entities.faces:
            self.assertEqual(face.front_material_id, material.id)

        # SketchUp 3 predates serialized face texture-coordinate objects.
        if not self.supports_feature(model, "face_uv_projection"):
            self.assertTrue(all(face.front_uv is None for face in model.entities.faces))
            return

        projections = [face.front_uv for face in model.entities.faces]
        self.assertTrue(all(projection is not None for projection in projections))
        self.assertTrue(
            all(len(projection.transform) == 9 for projection in projections)
        )

        # Scaled, rotated, mirrored, and projected inputs must not collapse to
        # one default mapping during save or parser normalization.
        transforms = {
            tuple(round(value, 9) for value in projection.transform)
            for projection in projections
        }
        self.assertGreaterEqual(len(transforms), 3)

        prepared = model.entities.prepare_mesh(
            "uv_positions",
            {material.id: material},
        )
        self.assertEqual(len(prepared.faces), 4)
        self.assertTrue(all(face.vertex_uvs is not None for face in prepared.faces))

        expected_uvs = (
            ((0.0, 0.0), (2.0, 0.0), (2.0, 1.0), (0.0, 1.0)),
            ((0.0, 1.0), (0.0, 0.0), (1.0, 0.0), (1.0, 1.0)),
            ((1.0, 0.0), (0.0, 0.0), (0.0, 1.0), (1.0, 1.0)),
            ((0.0, 0.0), (1.0, 0.0), (1.0, 1.0), (0.0, 1.0)),
        )
        for prepared_face, expected in zip(prepared.faces, expected_uvs):
            for actual_uv, expected_uv in zip(prepared_face.vertex_uvs, expected):
                self.assertAlmostEqual(actual_uv[0], expected_uv[0], places=6)
                self.assertAlmostEqual(actual_uv[1], expected_uv[1], places=6)


if __name__ == "__main__":
    unittest.main()
