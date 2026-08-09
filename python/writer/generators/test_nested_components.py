# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    child = model.add_definition("SubComponent")
    start = child.entities.add_vertex(0, 0, 0)
    end = child.entities.add_vertex(10, 0, 0)
    child.entities.add_edge(start, end)
    parent = model.add_definition("ParentComponent")
    parent.entities.add_instance(child, name="Nested Sub")
    model.entities.add_instance(parent, name="Root Parent")
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "nested_components"))
