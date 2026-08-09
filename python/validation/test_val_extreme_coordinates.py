# SPDX-License-Identifier: MIT
import numpy as np

from common import BaseSkpTest


class TestSkpExtremeCoordinates(BaseSkpTest):
    """Protect double precision at large coordinates and fractional values."""

    def test_extreme_coordinates_model(self):
        model = self.load_model("extreme_coordinates.skp")

        self.assert_entities_count(model.entities, vertices=6, edges=5, faces=1)
        points = np.array(
            [vertex.position.to_tuple() for vertex in model.entities.vertices]
        )
        expected = np.array(
            [
                [1000000.125, -1000000.25, 12345.5],
                [1000010.625, -1000000.25, 12345.5],
                [1000010.625, -999989.75, 12345.5],
                [1000000.125, -999989.75, 12345.5],
                [-1000000.5, 0.0009765625, -0.333333333333],
                [-999999.875, 0.001953125, 0.666666666667],
            ]
        )
        for expected_point in expected:
            distances = np.linalg.norm(points - expected_point, axis=1)
            self.assertLess(float(distances.min()), 1e-8)


if __name__ == "__main__":
    import unittest

    unittest.main()
