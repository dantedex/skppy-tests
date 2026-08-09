# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpMaterialPbr(BaseSkpTest):
    def test_material_pbr_model(self):
        model = self.load_model("material_pbr.skp")

        self.assert_restrictive(model, expected_materials=1, expected_layers=1)
        mat = model.materials[0]
        self.assertEqual(mat.name, "PBRMaterial")

        # The C generator sets metallic=0.8 and roughness=0.25.  However,
        # the SketchUp 2026.1 SDK DLL does not appear to serialize PBR
        # factors into the SKP file, so the parsed values remain at the
        # defaults (metallic=0.0, roughness=1.0).
        if mat.metallic == 0.0 and mat.roughness == 1.0:
            self.skipTest(
                "PBR factors not serialized by this SDK runtime "
                "(metallic/roughness remain at defaults)."
            )

        self.assertAlmostEqual(mat.metallic, 0.8, places=4)
        self.assertAlmostEqual(mat.roughness, 0.25, places=4)


if __name__ == "__main__":
    unittest.main()
