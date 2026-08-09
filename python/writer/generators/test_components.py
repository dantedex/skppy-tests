# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    objects = model.add_layer("Writer Objects")
    leaf = model.add_definition("Writer Leaf", description="Writer geometry")
    leaf.entities.add_face([(0, 0, 0), (1, 0, 0), (0, 1, 0)])
    parent = model.add_definition("Writer Parent")
    nested = parent.entities.add_instance(leaf, name="Writer Nested Leaf")
    nested.layer_id = objects.id
    instance = model.entities.add_instance(
        parent,
        skppy.Transform.from_translation(1, 2, 3),
        name="Writer Root Parent",
    )
    instance.layer_id = objects.id
    _, group = model.add_group(
        "Writer Group", skppy.Transform.from_translation(4, 5, 6)
    )
    group.layer_id = objects.id
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "components"))
