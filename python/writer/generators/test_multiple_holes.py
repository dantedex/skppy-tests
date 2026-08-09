# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import add_holes, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.add_face([(0, 0, 0), (120, 0, 0), (120, 80, 0), (0, 80, 0)])
    add_holes(
        model.entities,
        [
            [(10, 10, 0), (30, 10, 0), (30, 30, 0), (10, 30, 0)],
            [(50, 20, 0), (70, 20, 0), (70, 60, 0), (50, 60, 0)],
            [(90, 50, 0), (110, 50, 0), (110, 70, 0), (90, 70, 0)],
        ],
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "multiple_holes"))
