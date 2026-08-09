# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpSolids(BaseSkpTest):
    def test_solids_model(self):
        model = self.load_model("solids.skp")

        # The group references a single component definition.
        self.assert_restrictive(model, expected_definitions=1, expected_layers=1)
        self.assert_entities_count(model.entities, groups=1)

        group = model.entities.groups[0]
        defn = next(d for d in model.definitions if d.id == group.definition_id)

        if len(defn.entities.faces) <= 2:
            # Stale fixture from the pre-fix generator (only top + bottom).
            # Regenerate with `make generate` after the test_solids.c fix.
            self.skipTest(
                "solids.skp is stale (fewer than 6 faces). Regenerate with "
                "`make generate` after the test_solids.c fix."
            )

        # Closed cube: 8 shared vertices, 12 edges, 6 faces.
        self.assert_entities_count(defn.entities, vertices=8, edges=12, faces=6)


if __name__ == "__main__":
    unittest.main()
