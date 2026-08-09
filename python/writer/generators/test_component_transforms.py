# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    definition = model.add_definition("TransformComponent")
    start = definition.entities.add_vertex(0, 0, 0)
    end = definition.entities.add_vertex(1, 0, 0)
    definition.entities.add_edge(start, end)
    transforms = {
        "Translated": [1, 0, 0, 0, 1, 0, 0, 0, 1, 10, 20, 30, 1],
        "NonUniformScale": [2, 0, 0, 0, 3, 0, 0, 0, 4, -4, 5, 6, 1],
        "MirroredRotation": [0, 1, 0, 1, 0, 0, 0, 0, 1, -10, -20, -30, 1],
    }
    for name, values in transforms.items():
        model.entities.add_instance(definition, skppy.Transform(values), name=name)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "component_transforms"))
