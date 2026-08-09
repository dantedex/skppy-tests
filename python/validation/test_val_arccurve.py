# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpArcCurve(BaseSkpTest):
    def test_arccurve_model(self):
        model = self.load_model("arccurve.skp")

        # 1 arc -> 12 segments, 13 vertices, 12 edges, 1 arc curve
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=13, edges=12, arc_curves=1)

        arc = model.entities.arc_curves[0]
        self.assertEqual(len(arc.edge_ids), 12)


if __name__ == "__main__":
    unittest.main()
