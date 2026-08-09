# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpRendering(BaseSkpTest):
    def test_rendering_options(self):
        model = self.load_model("rendering_options.skp")
        self.assertIsNotNone(model.rendering_options)

        opts = model.rendering_options
        # In test_rendering_options.c we set:
        #   EdgeDisplayMode = false (0)
        #   DrawGround = true (1)
        self.assertIsNotNone(opts.edge_display_mode)
        self.assertEqual(opts.edge_display_mode, 0)
        self.assertTrue(opts.draw_ground)


if __name__ == "__main__":
    unittest.main()
