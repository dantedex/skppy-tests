# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpCircleUnfilled(BaseSkpTest):
    def test_circle_unfilled_model(self):
        model = self.load_model("circle_unfilled.skp")

        # 24-segment circle: 25 vertices (duplicated closing vertex),
        # 24 edges, 1 arc curve, 0 faces.
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(
            model.entities, vertices=25, edges=24, faces=0, arc_curves=1
        )

        arc = model.entities.arc_curves[0]
        self.assertEqual(len(arc.edge_ids), 24)


if __name__ == "__main__":
    unittest.main()
