# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    material = model.add_material(
        "Writer PBR",
        color=skppy.Color(12, 34, 56),
        alpha=0.4,
        metallic=0.75,
        roughness=0.125,
    )
    face = model.entities.add_face([(0, 0, 0), (2, 0, 0), (0, 2, 0)])
    face.front_material_id = material.id
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "material"))
