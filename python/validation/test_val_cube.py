# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpCube(BaseSkpTest):
    def test_cube_model(self):
        model = self.load_model("cube.skp")

        # 8 shared vertices, 12 edges, 6 faces.
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=8, edges=12, faces=6)


if __name__ == "__main__":
    unittest.main()
