# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import add_indexed_faces, run


POINTS = [
    (0, 0, 0),
    (10, 0, 0),
    (10, 10, 0),
    (0, 10, 0),
    (0, 0, 10),
    (10, 0, 10),
    (10, 10, 10),
    (0, 10, 10),
]
FACES = [
    (3, 2, 1, 0),
    (4, 5, 6, 7),
    (0, 1, 5, 4),
    (2, 3, 7, 6),
    (1, 2, 6, 5),
    (3, 0, 4, 7),
]


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    add_indexed_faces(skppy, model.entities, POINTS, FACES)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "cube"))
