# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    for index, flags in enumerate((0, 0x02, 0x04, 0x01)):
        start = model.entities.add_vertex(0, index * 10, 0)
        end = model.entities.add_vertex(5, index * 10, 0)
        model.entities.add_edge(start, end).flags = flags
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "edge_flags"))
