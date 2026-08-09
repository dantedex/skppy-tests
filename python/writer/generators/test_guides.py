# SPDX-License-Identifier: MIT
import math
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.guide_points.append(skppy.GuidePoint(id=1, position=(10, 20, 30)))
    component = 1.0 / math.sqrt(3.0)
    model.entities.guide_lines.append(
        skppy.GuideLine(
            id=2,
            point=(0, 0, 0),
            direction=(component, component, component),
            start_parameter=0.0,
            end_parameter=100.0 * math.sqrt(3.0),
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "guides"))
