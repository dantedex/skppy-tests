# SPDX-License-Identifier: MIT
import math
import unittest

from common import BaseSkpTest


def _coordinates(value):
    """Return coordinates from either supported guide-vector representation."""
    return value.to_tuple() if hasattr(value, "to_tuple") else value


class TestSkpGuides(BaseSkpTest):
    def test_guides_model(self):
        model = self.load_model("guides.skp")

        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, guide_points=1, guide_lines=1)

        # Guide point at (10, 20, 30)
        gp = model.entities.guide_points[0]
        self.assertEqual(tuple(_coordinates(gp.position)), (10.0, 20.0, 30.0))

        # Finite guide line from (0,0,0) to (100,100,100).
        # SketchUp stores a point on the line and the unit direction.
        gl = model.entities.guide_lines[0]
        self.assertEqual(tuple(_coordinates(gl.point)), (0.0, 0.0, 0.0))

        # Direction is the normalized (1,1,1) vector.
        inv = 1.0 / math.sqrt(3.0)
        for component in _coordinates(gl.direction):
            self.assertAlmostEqual(component, inv, places=4)


if __name__ == "__main__":
    unittest.main()
