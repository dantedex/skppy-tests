# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    definition = model.add_definition("Billboard")
    definition.behavior_always_face_camera = True
    definition.entities.add_face([(-5, 0, 0), (5, 0, 0), (5, 0, 10), (-5, 0, 10)])
    model.entities.add_instance(definition, name="Billboard Instance")
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "billboard"))
