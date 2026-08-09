# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    start = model.entities.add_vertex(1.0e12 + 0.125, -1.0e12 - 0.25, 0.375)
    end = model.entities.add_vertex(1.0e12 + 9.5, -1.0e12 + 8.25, -7.125)
    model.entities.add_edge(start, end)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "extreme_coordinates"))
