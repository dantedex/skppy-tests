# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpTextures(BaseSkpTest):
    def test_textures_model(self):
        model = self.load_model("textures.skp")

        # 1 material, 1 face -> 4 edges, 4 vertices
        self.assert_restrictive(model, expected_materials=1, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=4, edges=4, faces=1)

        mat = model.materials[0]
        self.assertEqual(mat.name, "TexturedMaterial")
        # Texture may or may not be present depending on whether the resource
        # file was found when the .skp was generated.
        if mat.texture is not None:
            self.assertIn("brick_wall", mat.texture.filename.lower())


if __name__ == "__main__":
    unittest.main()
