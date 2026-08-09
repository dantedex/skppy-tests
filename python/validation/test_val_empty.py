# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpEmpty(BaseSkpTest):
    def test_empty_model(self):
        model = self.load_model("empty.skp")

        # Restrictive checks
        # expected_layers is usually 1 (Layer0).
        # SketchUp always saves a default camera, even for an empty model.
        self.assert_restrictive(
            model,
            expected_entities=0,
            expected_materials=0,
            expected_definitions=0,
            expected_layers=1,
            expected_scenes=0,
            expected_cameras=1,
        )

        self.assert_entities_count(model.entities, vertices=0, edges=0, faces=0)


if __name__ == "__main__":
    unittest.main()
