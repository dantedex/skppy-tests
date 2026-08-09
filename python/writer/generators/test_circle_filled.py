# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import circle_points, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.add_face(circle_points(10.0))
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "circle_filled"))
