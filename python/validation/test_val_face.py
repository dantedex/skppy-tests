# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpFace(BaseSkpTest):
    def test_face_model(self):
        model = self.load_model("face.skp")

        # 1 face -> 4 edges, 4 vertices
        self.assert_restrictive(
            model, expected_materials=0, expected_definitions=0, expected_layers=1
        )
        self.assert_entities_count(model.entities, vertices=4, edges=4, faces=1)

        face = model.entities.faces[0]
        self.assertEqual(len(face.outer_loop.edge_uses), 4)

        # Check one vertex
        v0 = model.entities.vertices[0]
        self.assertIn(v0.position.x, [0, 100])
        self.assertIn(v0.position.y, [0, 100])
        self.assertEqual(v0.position.z, 0)


if __name__ == "__main__":
    unittest.main()
