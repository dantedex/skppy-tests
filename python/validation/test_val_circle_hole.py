# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpCircleHole(BaseSkpTest):
    def test_circle_hole_model(self):
        model = self.load_model("circle_hole.skp")

        # 24-segment outer + 24-segment inner hole: 48 vertices, 48 edges,
        # 1 face with 1 inner loop.
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=48, edges=48, faces=1)

        face = model.entities.faces[0]
        self.assertEqual(len(face.outer_loop.edge_uses), 24)
        self.assertEqual(len(face.inner_loops), 1)
        self.assertEqual(len(face.inner_loops[0].edge_uses), 24)


if __name__ == "__main__":
    unittest.main()
