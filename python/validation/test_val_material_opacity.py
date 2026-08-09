# SPDX-License-Identifier: MIT
from common import BaseSkpTest


class TestSkpMaterialOpacity(BaseSkpTest):
    """Distinguish stored opacity from SketchUp's use-opacity switch."""

    def test_material_opacity_model(self):
        model = self.load_model("material_opacity.skp")

        self.assert_entities_count(model.entities, vertices=12, edges=12, faces=3)
        materials = {material.name: material for material in model.materials}
        self.assertAlmostEqual(materials["OpacityIgnored"].alpha, 1.0)
        self.assertAlmostEqual(materials["Translucent"].alpha, 0.35)
        self.assertAlmostEqual(materials["FullyTransparent"].alpha, 0.0)


if __name__ == "__main__":
    import unittest

    unittest.main()
