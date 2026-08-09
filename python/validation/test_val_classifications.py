# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpClassifications(BaseSkpTest):
    def test_classifications_model(self):
        model = self.load_model("classifications.skp")

        # test_classifications.c currently only opens the classifications API
        # but doesn't add any classification data.
        # Once classification parsing is implemented, validate the data here.
        self.assert_restrictive(model, expected_layers=1)


if __name__ == "__main__":
    unittest.main()
