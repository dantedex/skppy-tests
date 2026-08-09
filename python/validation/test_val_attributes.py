# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpAttributes(BaseSkpTest):
    def test_attributes_model(self):
        model = self.load_model("attributes.skp")

        # 1 edge -> 2 vertices
        self.assert_restrictive(
            model, expected_materials=1, expected_definitions=0, expected_layers=2
        )
        self.assert_entities_count(model.entities, vertices=2, edges=1)

        edge = model.entities.edges[0]
        dictionaries = model.entities.attribute_dictionaries_by_entity_id.get(
            edge.id, []
        )
        if self.supports_feature(model, "entity_attribute_dictionaries"):
            self.assertEqual(
                [dictionary.name for dictionary in dictionaries], ["TestData"]
            )
            self.assertEqual(dictionaries[0].entries[0].key, "Message")
            self.assertEqual(dictionaries[0].entries[0].string_value, "Hello SketchUp")
        else:
            self.assertEqual(dictionaries, [])

        # Verify the edge geometry
        v_start = next(
            v for v in model.entities.vertices if v.id == edge.start_vertex_id
        )
        v_end = next(v for v in model.entities.vertices if v.id == edge.end_vertex_id)
        self.assertEqual(v_start.position.x, 0)
        self.assertEqual(v_end.position.x, 10)
        vertex_dictionaries = model.entities.attribute_dictionaries_by_entity_id.get(
            v_start.id, []
        )
        self.assertEqual(vertex_dictionaries[0].name, "VertexData")
        self.assertEqual(vertex_dictionaries[0].entries[0].string_value, "start")

        layer = next(layer for layer in model.layers if layer.name == "AttributedLayer")
        layer_dictionaries = model.attribute_dictionaries_by_object_id[layer.id]
        self.assertEqual(layer_dictionaries[0].name, "LayerData")
        self.assertEqual(layer_dictionaries[0].entries[0].string_value, "architecture")


if __name__ == "__main__":
    unittest.main()
