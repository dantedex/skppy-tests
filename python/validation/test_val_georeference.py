# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpGeoreference(BaseSkpTest):
    def test_georeference_model(self):
        model = self.load_model("georeference.skp")

        self.assertIsNotNone(model.shadow_info)
        self.assertEqual(model.shadow_info.latitude, 45.0)
        self.assertEqual(model.shadow_info.longitude, -120.0)


if __name__ == "__main__":
    unittest.main()
