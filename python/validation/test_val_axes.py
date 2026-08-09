# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpAxes(BaseSkpTest):
    def test_axes_model(self):
        model = self.load_model("axes.skp")

        self.assertIsNotNone(model.model_view_axes)
        self.assertEqual(model.model_view_axes.origin[0], 10)
        self.assertEqual(model.model_view_axes.origin[1], 10)
        self.assertEqual(model.model_view_axes.origin[2], 10)


if __name__ == "__main__":
    unittest.main()
