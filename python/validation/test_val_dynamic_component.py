# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpDynamicComponent(BaseSkpTest):
    def test_dynamic_component_model(self):
        model = self.load_model("dynamic_component.skp")
        self.assert_restrictive(model, expected_definitions=1, expected_layers=1)

        defn = model.definitions[0]
        self.assertEqual(defn.name, "DynamicComponent")

        dictionaries = model.attribute_dictionaries_by_object_id.get(defn.id, [])
        if self.supports_feature(model, "attribute_dictionaries"):
            self.assertEqual(
                [dictionary.name for dictionary in dictionaries],
                ["dynamic_attributes"],
            )
            self.assertEqual(dictionaries[0].entries[0].key, "_lenx_nominal")
            self.assertEqual(dictionaries[0].entries[0].string_value, "100")
        else:
            self.assertEqual(dictionaries, [])


if __name__ == "__main__":
    unittest.main()
