# SPDX-License-Identifier: MIT
import numpy as np

from common import BaseSkpTest


class TestSkpComponentTransforms(BaseSkpTest):
    """Validate translation, non-uniform scale, rotation, and mirroring."""

    def test_component_transforms_model(self):
        model = self.load_model("component_transforms.skp")

        self.assert_restrictive(model, expected_definitions=1, expected_layers=1)
        self.assert_entities_count(model.entities, instances=3)
        serialized_instances = model.entities.component_instances
        if self.supports_feature(model, "component_instance_name"):
            instances = {instance.name: instance for instance in serialized_instances}
        else:
            # CComponentInstance v3 (SU3-SU4) has no serialized name. Archive
            # order is stable and the transforms themselves remain available.
            self.assertEqual(
                [instance.name for instance in serialized_instances],
                [None] * 3,
            )
            instances = dict(
                zip(
                    ("Translated", "NonUniformScale", "MirroredRotation"),
                    serialized_instances,
                    strict=True,
                )
            )

        np.testing.assert_allclose(
            instances["Translated"].transform,
            [1, 0, 0, 0, 1, 0, 0, 0, 1, 10, 20, 30, 1],
        )
        np.testing.assert_allclose(
            instances["NonUniformScale"].transform,
            [2, 0, 0, 0, 3, 0, 0, 0, 4, -4, 5, 6, 1],
        )
        np.testing.assert_allclose(
            instances["MirroredRotation"].transform,
            [0, 1, 0, 1, 0, 0, 0, 0, 1, -10, -20, -30, 1],
        )


if __name__ == "__main__":
    import unittest

    unittest.main()
