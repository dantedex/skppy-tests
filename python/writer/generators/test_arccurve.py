# SPDX-License-Identifier: MIT
import math
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.add_arc_curve(
        center=(0, 0, 0),
        normal=(0, 0, 1),
        radius=10.0,
        start_angle=0.0,
        end_angle=math.pi / 2.0,
        segments=12,
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "arccurve"))
