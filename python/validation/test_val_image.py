# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpImage(BaseSkpTest):
    def test_image_model(self):
        model = self.load_model("image.skp")

        # The C generator loads a resource PNG as an image.  When the resource
        # is found, the model has 1 image, 1 definition (the image's backing
        # component), and 1 material (the image's material).
        if len(model.entities.images) > 0:
            self.assert_entities_count(model.entities, images=1)
            self.assert_restrictive(
                model, expected_materials=1, expected_definitions=1, expected_layers=1
            )
        else:
            self.assert_restrictive(model, expected_layers=1)


if __name__ == "__main__":
    unittest.main()
