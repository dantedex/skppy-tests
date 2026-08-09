# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpQuarterCircle(BaseSkpTest):
    def test_quarter_circle_model(self):
        model = self.load_model("quarter_circle.skp")

        # 6-segment quarter circle (90-degree arc): 7 vertices (6 segments +
        # 1 closing vertex), 6 edges, 1 arc curve.
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=7, edges=6, arc_curves=1)

        arc = model.entities.arc_curves[0]
        self.assertEqual(len(arc.edge_ids), 6)


if __name__ == "__main__":
    unittest.main()
