# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpOpeningCut(BaseSkpTest):
    def test_opening_cut_model(self):
        model = self.load_model("opening_cut.skp")
        self.assert_restrictive(model, expected_definitions=1, expected_layers=1)

        defn = model.definitions[0]
        self.assertEqual(defn.name, "OpeningComponent")
        self.assertTrue(defn.behavior_cuts_opening)


if __name__ == "__main__":
    unittest.main()
