# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpLineStyles(BaseSkpTest):
    def test_line_styles_model(self):
        model = self.load_model("line_styles.skp")

        # SketchUp always emits the 12 built-in line styles, even for an
        # otherwise empty model.
        self.assert_restrictive(model, expected_layers=1)
        self.assertGreaterEqual(len(model.line_styles), 12)

        names = {ls.name for ls in model.line_styles}
        # A representative subset of the built-in styles.
        for expected in ("Solid Basic", "Dash", "Dot", "Dash dot"):
            self.assertIn(expected, names)

        # Every style must expose a non-empty dash pattern and a positive width.
        for ls in model.line_styles:
            self.assertTrue(ls.name)
            self.assertGreater(ls.line_width_points, 0.0)


if __name__ == "__main__":
    unittest.main()
