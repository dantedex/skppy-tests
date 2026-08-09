# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpComponentBehavior(BaseSkpTest):
    def test_component_behavior_model(self):
        model = self.load_model("component_behavior.skp")
        self.assert_restrictive(model, expected_definitions=1, expected_layers=1)

        defn = model.definitions[0]
        self.assertEqual(defn.name, "BehaviorComponent")
        self.assertEqual(
            defn.behavior_always_face_camera,
            self.supports_feature(model, "component_behavior"),
        )


if __name__ == "__main__":
    unittest.main()
