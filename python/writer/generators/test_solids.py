# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import add_indexed_faces, run
from .test_cube import FACES, POINTS


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    definition, _ = model.add_group("Writer Solid")
    add_indexed_faces(skppy, definition.entities, POINTS, FACES)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "solids"))
