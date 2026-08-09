# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpNested(BaseSkpTest):
    def test_nested_model(self):
        model = self.load_model("nested_components.skp")

        # 2 definitions, 1 layer, 1 instance in root
        self.assert_restrictive(model, expected_definitions=2, expected_layers=1)
        self.assert_entities_count(model.entities, instances=1)

        parent_def = next(d for d in model.definitions if d.name == "ParentComponent")
        sub_def = next(d for d in model.definitions if d.name == "SubComponent")

        # Parent contains Sub instance
        self.assert_entities_count(parent_def.entities, instances=1)

        # Sub contains Edge
        self.assert_entities_count(sub_def.entities, vertices=2, edges=1)


if __name__ == "__main__":
    unittest.main()
