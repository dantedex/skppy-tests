# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpSectionPlanes(BaseSkpTest):
    def test_section_planes_model(self):
        model = self.load_model("section_planes.skp")

        self.assert_restrictive(model, expected_layers=1)
        self.assert_entities_count(model.entities, section_planes=1)

        sp = model.entities.section_planes[0]
        self.assertEqual(
            sp.name,
            (
                "TestSection"
                if self.supports_feature(model, "section_plane_name")
                else ""
            ),
        )


if __name__ == "__main__":
    unittest.main()
