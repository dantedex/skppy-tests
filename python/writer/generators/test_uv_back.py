# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    front = model.add_material("Writer Front", color=skppy.Color(190, 20, 30))
    back = model.add_material("Writer Back", color=skppy.Color(30, 40, 190))
    face = model.entities.add_face([(0, 0, 0), (2, 0, 0), (0, 2, 0)])
    face.front_material_id = front.id
    face.back_material_id = back.id
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "uv_back"))
