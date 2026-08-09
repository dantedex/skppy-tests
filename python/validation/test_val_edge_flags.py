# SPDX-License-Identifier: MIT
from common import BaseSkpTest


class TestSkpEdgeFlags(BaseSkpTest):
    """Keep soft, smooth, and hidden flags independent during parsing."""

    def test_edge_flags_model(self):
        model = self.load_model("edge_flags.skp")

        self.assert_entities_count(model.entities, vertices=8, edges=4)
        vertices = {vertex.id: vertex.position for vertex in model.entities.vertices}
        edges_by_y = {
            round(vertices[edge.start_vertex_id].y): edge
            for edge in model.entities.edges
        }

        normal = edges_by_y[0]
        self.assertFalse(normal.is_soft)
        self.assertFalse(normal.is_smooth)
        self.assertFalse(normal.is_hidden)

        soft = edges_by_y[10]
        self.assertTrue(soft.is_soft)
        self.assertFalse(soft.is_smooth)
        self.assertFalse(soft.is_hidden)

        smooth = edges_by_y[20]
        self.assertFalse(smooth.is_soft)
        self.assertTrue(smooth.is_smooth)
        self.assertFalse(smooth.is_hidden)

        hidden = edges_by_y[30]
        self.assertFalse(hidden.is_soft)
        self.assertFalse(hidden.is_smooth)
        self.assertTrue(hidden.is_hidden)


if __name__ == "__main__":
    import unittest

    unittest.main()
