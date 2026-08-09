# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


class TestSkpEnvironment(BaseSkpTest):
    def test_environment_model(self):
        model = self.load_model("environment.skp")

        # The environment block is always present (SketchUp emits an empty
        # EnvironmentData even when no environment is selected).
        self.assertIsNotNone(model.environment_data)
        self.assert_restrictive(model, expected_layers=1)

        # The C generator tries to create an environment from studio.exr.
        # If the SDK runtime supports it, entries will be populated; otherwise
        # the block is empty (SDK limitation).
        env = model.environment_data
        if not env.entries:
            self.skipTest(
                "Environment entries not populated (SDK runtime limitation "
                "or resource not found)."
            )

        names = [e.name for e in env.entries]
        self.assertIn("StudioEnvironment", names)


if __name__ == "__main__":
    unittest.main()
