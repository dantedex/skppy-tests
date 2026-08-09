# SPDX-License-Identifier: MIT
from common import BaseSkpTest


class TestSkpMultipleHoles(BaseSkpTest):
    """Validate a face whose boundary contains three independent holes."""

    def test_multiple_holes_model(self):
        model = self.load_model("multiple_holes.skp")

        self.assert_entities_count(model.entities, vertices=16, edges=16, faces=1)
        face = model.entities.faces[0]
        self.assertEqual(len(face.outer_loop.edge_uses), 4)
        self.assertEqual(len(face.inner_loops), 3)
        self.assertEqual(
            [len(loop.edge_uses) for loop in face.inner_loops],
            [4, 4, 4],
        )


if __name__ == "__main__":
    import unittest

    unittest.main()
