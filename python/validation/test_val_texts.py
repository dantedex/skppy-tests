# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpTexts(BaseSkpTest):
    def test_texts_model(self):
        model = self.load_model("texts.skp")

        # The SketchUp 2026.1 SDK DLL cannot serialize text entities, so the
        # generator falls back to saving an empty model.  skppy does not yet
        # parse text entities into a dedicated list.  Once both the SDK and
        # skppy support texts, this test should check:
        #   self.assertEqual(len(model.entities.texts), 1)
        #   self.assertEqual(model.entities.texts[0].string, "Hello, World!")
        # For now, just verify the file loads without error.
        self.assertIsNotNone(model)


if __name__ == "__main__":
    unittest.main()
