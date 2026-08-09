# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpBillboard(BaseSkpTest):
    def test_billboard_model(self):
        model = self.load_model("billboard.skp")

        # 1 definition (Billboard), 1 instance in root.
        self.assert_restrictive(model, expected_definitions=1, expected_layers=1)
        self.assert_entities_count(model.entities, instances=1)

        defn = model.definitions[0]
        self.assertEqual(defn.name, "Billboard")
        self.assertEqual(
            defn.behavior_always_face_camera,
            self.supports_feature(model, "component_behavior"),
        )

        # The definition contains a single face (4 verts, 4 edges).
        self.assert_entities_count(defn.entities, vertices=4, edges=4, faces=1)


if __name__ == "__main__":
    unittest.main()
