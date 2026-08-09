# SPDX-License-Identifier: MIT
import math
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.add_arc_curve((0, 0, 0), (0, 0, 1), 10.0, 0.0, 2.0 * math.pi, 24)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "circle"))
