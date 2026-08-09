# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import add_indexed_faces, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    add_indexed_faces(
        skppy,
        model.entities,
        [(0, 0, 0), (10, 0, 0), (10, 10, 0), (0, 10, 0), (5, 5, 10)],
        [(3, 2, 1, 0), (0, 1, 4), (1, 2, 4), (2, 3, 4), (3, 0, 4)],
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "pyramid"))
