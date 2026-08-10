# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpDimensions(BaseSkpTest):
    def test_dimensions_model(self):
        model = self.load_model("dimensions.skp")

        self.assertEqual(len(model.entities.linear_dimensions), 1)
        linear = model.entities.linear_dimensions[0]
        self.assertEqual(linear.start.kind, 5)
        if self.supports_feature(model, "dimension_point_reference_instance_path"):
            self.assertIsNotNone(linear.start.entity_id)
            self.assertEqual(len(linear.start.instance_path_ids), 1)
        else:
            self.assertIsNone(linear.start.entity_id)
            self.assertEqual(linear.start.instance_path_ids, [])
        self.assertIsNotNone(linear.end.entity_id)
        self.assertEqual(linear.end.instance_path_ids, [])
        self.assertFalse(linear.drawing.casts_shadows)
        self.assertTrue(linear.drawing.receives_shadows)
        self.assertEqual(len(model.entities.radial_dimensions), 1)
        self.assertTrue(model.entities.radial_dimensions[0].drawing.casts_shadows)
        self.assertFalse(model.entities.radial_dimensions[0].drawing.receives_shadows)


if __name__ == "__main__":
    unittest.main()
