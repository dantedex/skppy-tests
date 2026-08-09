# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpCircle(BaseSkpTest):
    def test_circle_model(self):
        model = self.load_model("circle.skp")

        # 1 circle -> 24 segments.  SketchUp emits 25 vertices (the start
        # vertex is duplicated to close the loop), 24 edges, 1 arc curve.
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=25, edges=24, arc_curves=1)

        arc = model.entities.arc_curves[0]
        self.assertEqual(len(arc.edge_ids), 24)


if __name__ == "__main__":
    unittest.main()
