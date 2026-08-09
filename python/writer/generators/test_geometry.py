# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.add_face([(0, 0, 0), (4, 0, 0), (0, 3, 0)])
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "geometry"))
