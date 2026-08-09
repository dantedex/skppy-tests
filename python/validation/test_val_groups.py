# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpGroups(BaseSkpTest):
    def test_groups_model(self):
        model = self.load_model("groups.skp")

        # 3 groups in root.  Each group creates its own definition, plus the
        # explicit "GroupedComponent" definition, plus the nested group's
        # definition.  So we don't assert an exact definition count - we
        # verify the structure instead.
        self.assert_restrictive(model, expected_layers=1, expected_definitions=-1)
        self.assert_entities_count(model.entities, groups=3)

        # Group 0: a square face (4 verts, 4 edges, 1 face)
        g0 = model.entities.groups[0]
        defn0 = next(d for d in model.definitions if d.id == g0.definition_id)
        self.assert_entities_count(defn0.entities, vertices=4, edges=4, faces=1)

        # Group 1: contains a nested group with an edge (2 verts, 1 edge)
        g1 = model.entities.groups[1]
        defn1 = next(d for d in model.definitions if d.id == g1.definition_id)
        self.assert_entities_count(defn1.entities, groups=1)
        inner = defn1.entities.groups[0]
        inner_def = next(d for d in model.definitions if d.id == inner.definition_id)
        self.assert_entities_count(inner_def.entities, vertices=2, edges=1)

        # Group 2: contains a component instance referencing "GroupedComponent"
        g2 = model.entities.groups[2]
        defn2 = next(d for d in model.definitions if d.id == g2.definition_id)
        self.assert_entities_count(defn2.entities, instances=1)

        # The referenced component definition should exist by name.
        comp_def = next(d for d in model.definitions if d.name == "GroupedComponent")
        self.assert_entities_count(comp_def.entities, vertices=2, edges=1)


if __name__ == "__main__":
    unittest.main()
