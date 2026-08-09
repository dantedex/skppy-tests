# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    material = model.add_material("RedMaterial", color=skppy.Color(255, 0, 0))
    face = model.entities.add_face([(0, 0, 0), (10, 0, 0), (10, 10, 0), (0, 10, 0)])
    face.front_material_id = material.id
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "materials"))
