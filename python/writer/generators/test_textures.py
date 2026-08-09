# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import one_pixel_png, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    material = model.add_material("TexturedMaterial", color=skppy.Color(192, 192, 192))
    material.has_texture = True
    material.texture = skppy.Texture(
        filename="brick_wall.png",
        x_scale=100.0,
        y_scale=100.0,
        data=one_pixel_png(),
    )
    face = model.entities.add_face([(0, 0, 0), (100, 0, 0), (100, 100, 0), (0, 100, 0)])
    face.front_material_id = material.id
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "textures"))
