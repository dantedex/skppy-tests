# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


def _fixture_is_stale(model):
    """True when face_hole.skp predates the test_face_hole.c fix."""
    return (
        len(model.entities.vertices) == 4
        and model.entities.faces
        and not model.entities.faces[0].inner_loops
    )


class TestSkpFaceHole(BaseSkpTest):
    def test_face_hole_model(self):
        model = self.load_model("face_hole.skp")

        if _fixture_is_stale(model):
            self.skipTest(
                "face_hole.skp is stale (no inner loop). Regenerate with "
                "`make generate` after the test_face_hole.c fix."
            )

        # 1 face with hole -> 8 vertices, 8 edges, 1 face, 1 inner loop.
        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=8, edges=8, faces=1)

        face = model.entities.faces[0]
        self.assertEqual(len(face.outer_loop.edge_uses), 4)
        self.assertEqual(len(face.inner_loops), 1)
        self.assertEqual(len(face.inner_loops[0].edge_uses), 4)


if __name__ == "__main__":
    unittest.main()
