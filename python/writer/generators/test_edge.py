# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    start = model.entities.add_vertex(1.25, -2.5, 3.75)
    end = model.entities.add_vertex(9.5, 8.25, -7.0)
    model.entities.add_edge(start, end)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "edge"))
