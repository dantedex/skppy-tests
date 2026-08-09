# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpEdge(BaseSkpTest):
    def test_edge_model(self):
        model = self.load_model("edge.skp")

        # 1 edge -> 2 vertices
        self.assert_restrictive(
            model, expected_materials=0, expected_definitions=0, expected_layers=1
        )
        self.assert_entities_count(model.entities, vertices=2, edges=1)

        edge = model.entities.edges[0]
        v_start = next(
            v for v in model.entities.vertices if v.id == edge.start_vertex_id
        )
        v_end = next(v for v in model.entities.vertices if v.id == edge.end_vertex_id)

        self.assertEqual(v_start.position.x, 0)
        self.assertEqual(v_start.position.y, 0)
        self.assertEqual(v_start.position.z, 0)

        self.assertEqual(v_end.position.x, 10)
        self.assertEqual(v_end.position.y, 10)
        self.assertEqual(v_end.position.z, 10)


if __name__ == "__main__":
    unittest.main()
