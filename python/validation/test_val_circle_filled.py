# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpCircleFilled(BaseSkpTest):
    def test_circle_filled_model(self):
        model = self.load_model("circle_filled.skp")

        # 24-segment filled circle (disk): 24 vertices, 24 edges, 1 face.
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=24, edges=24, faces=1)

        face = model.entities.faces[0]
        self.assertEqual(len(face.outer_loop.edge_uses), 24)
        self.assertEqual(len(face.inner_loops), 0)


if __name__ == "__main__":
    unittest.main()
