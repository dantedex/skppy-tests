# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpMaterials(BaseSkpTest):
    def test_materials_model(self):
        model = self.load_model("materials.skp")

        # 1 material, 1 face -> 4 edges, 4 vertices
        self.assert_restrictive(model, expected_materials=1, expected_layers=1)
        self.assert_entities_count(model.entities, vertices=4, edges=4, faces=1)

        mat = model.materials[0]
        self.assertEqual(mat.name, "RedMaterial")
        self.assertEqual(mat.color.r, 255)
        self.assertEqual(mat.color.g, 0)
        self.assertEqual(mat.color.b, 0)

        face = model.entities.faces[0]
        self.assertEqual(face.front_material_id, mat.id)


if __name__ == "__main__":
    unittest.main()
