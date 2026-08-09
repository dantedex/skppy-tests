# SPDX-License-Identifier: MIT
from common import BaseSkpTest


class TestSkpUnicodeNames(BaseSkpTest):
    """Validate UTF-8 names across independent model object families."""

    def test_unicode_names_model(self):
        model = self.load_model("unicode_names.skp")

        self.assertIn("Camada 東京", {layer.name for layer in model.layers})
        self.assertIn("Material Café", {material.name for material in model.materials})

        definition = next(
            definition
            for definition in model.definitions
            if definition.name == "Componente Ω"
        )
        self.assertEqual(definition.description, "Descrição UTF-8")

        instance = model.entities.component_instances[0]
        self.assertEqual(
            instance.name,
            (
                "Instância ☃"
                if self.supports_feature(model, "component_instance_name")
                else None
            ),
        )

        scene = next(scene for scene in model.scenes if scene.name == "Cena São Paulo")
        self.assertEqual(scene.description, "Visão principal")


if __name__ == "__main__":
    import unittest

    unittest.main()
