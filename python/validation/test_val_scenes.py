# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpScenes(BaseSkpTest):
    def test_scenes_model(self):
        model = self.load_model("scenes.skp")

        # 1 scene, 1 layer
        # Note: model.cameras might have the root camera too.
        self.assert_restrictive(model, expected_scenes=1, expected_layers=1)

        scene = model.scenes[0]
        self.assertEqual(scene.name, "TestScene")


if __name__ == "__main__":
    unittest.main()
