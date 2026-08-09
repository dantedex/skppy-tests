# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpRenderingSectionDisplay(BaseSkpTest):
    def test_section_display_flags(self):
        planes_false = self.load_model("rendering_section_planes_false.skp")
        planes_true = self.load_model("rendering_section_planes_true.skp")
        cuts_false = self.load_model("rendering_section_cuts_false.skp")
        cuts_true = self.load_model("rendering_section_cuts_true.skp")

        self.assertFalse(planes_false.rendering_options.display_section_planes)
        self.assertTrue(planes_true.rendering_options.display_section_planes)
        self.assertFalse(cuts_false.rendering_options.display_section_cuts)
        self.assertTrue(cuts_true.rendering_options.display_section_cuts)

        # The independent options share one serialized mask. Toggling either
        # key must preserve the other key's default state.
        self.assertTrue(planes_false.rendering_options.display_section_cuts)
        self.assertTrue(planes_true.rendering_options.display_section_cuts)
        self.assertTrue(cuts_false.rendering_options.display_section_planes)
        self.assertTrue(cuts_true.rendering_options.display_section_planes)


if __name__ == "__main__":
    unittest.main()
