# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import add_holes, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.add_face([(0, 0, 0), (100, 0, 0), (100, 100, 0), (0, 100, 0)])
    add_holes(
        model.entities,
        [[(25, 25, 0), (75, 25, 0), (75, 75, 0), (25, 75, 0)]],
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "face_hole"))
