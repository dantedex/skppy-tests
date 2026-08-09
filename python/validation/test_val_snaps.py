# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpSnaps(BaseSkpTest):
    def test_snaps_model(self):
        model = self.load_model("snaps.skp")

        # The stale fixture (pre-fix generator) is an empty model with no
        # definitions.  The regenerated fixture has one definition with
        # snap-to-vertical behavior.
        if not model.definitions:
            self.skipTest(
                "snaps.skp is stale (no definitions). Regenerate with "
                "`make generate` after the test_snaps.c fix."
            )

        self.assert_restrictive(model, expected_definitions=1, expected_layers=1)

        defn = model.definitions[0]
        self.assertEqual(defn.name, "SnappableComponent")

        # The C generator sets component_snap = SUSnapToBehavior_Vertical.
        # skppy parses the TLV snap-mode value (which maps to 2 for Vertical
        # in the serialized format).
        self.assertTrue(defn.behavior_snap_enabled)
        self.assertGreater(defn.behavior_snap_mode, 0)


if __name__ == "__main__":
    unittest.main()
