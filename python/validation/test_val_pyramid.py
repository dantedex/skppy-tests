# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpPyramid(BaseSkpTest):
    def test_pyramid_model(self):
        model = self.load_model("pyramid.skp")

        # 4-sided pyramid: 5 vertices (4 base + 1 apex), 8 edges
        # (4 base + 4 sides), 5 faces (1 base + 4 triangles).
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=5, edges=8, faces=5)


if __name__ == "__main__":
    unittest.main()
