# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    specifications = (
        ("OpacityIgnored", skppy.Color(220, 40, 40), 1.0),
        ("Translucent", skppy.Color(40, 220, 40), 0.35),
        ("FullyTransparent", skppy.Color(40, 40, 220), 0.0),
    )
    for index, (name, color, alpha) in enumerate(specifications):
        material = model.add_material(name, color=color, alpha=alpha)
        x = index * 20
        face = model.entities.add_face(
            [(x, 0, 0), (x + 10, 0, 0), (x + 10, 10, 0), (x, 10, 0)]
        )
        face.front_material_id = material.id
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "material_opacity"))
