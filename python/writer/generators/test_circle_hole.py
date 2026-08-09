# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import add_holes, circle_points, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.add_face(circle_points(20.0))
    add_holes(model.entities, [circle_points(5.0)])
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "circle_hole"))
