# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpCylinder(BaseSkpTest):
    def test_cylinder_model(self):
        model = self.load_model("cylinder.skp")

        # Hollow cylinder (tube) matching the reference cilynder.skp:
        #   - 4 ArcCurve entities (bottom-outer, top-outer, bottom-inner, top-inner)
        #   - 96 vertices (24 segments x 4 circles)
        #   - 144 edges (24x4 circle edges + 24x2 side edges, shared)
        #   - 50 faces (2 caps with holes + 24 outer sides + 24 inner sides)
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(
            model.entities,
            vertices=96,
            edges=144,
            faces=50,
            arc_curves=4,
        )

        # Each arc curve should reference 24 edges.
        for arc in model.entities.arc_curves:
            self.assertEqual(len(arc.edge_ids), 24)

        # The two cap faces should each have an inner loop (hole).
        caps_with_holes = [f for f in model.entities.faces if len(f.inner_loops) > 0]
        self.assertEqual(len(caps_with_holes), 2)
        for cap in caps_with_holes:
            self.assertEqual(len(cap.outer_loop.edge_uses), 24)
            self.assertEqual(len(cap.inner_loops), 1)
            self.assertEqual(len(cap.inner_loops[0].edge_uses), 24)


if __name__ == "__main__":
    unittest.main()
