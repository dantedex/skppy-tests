# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import one_pixel_png, run


def _textured_material(skppy, model, name: str, filename: str, color):
    material = model.add_material(name, color=color)
    material.has_texture = True
    material.texture = skppy.Texture(
        filename=filename,
        x_scale=100.0,
        y_scale=100.0,
        data=one_pixel_png(),
    )
    return material


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    front = _textured_material(
        skppy, model, "FrontMaterial", "front.png", skppy.Color(200, 50, 50)
    )
    back = _textured_material(
        skppy, model, "BackMaterial", "back.png", skppy.Color(50, 50, 200)
    )
    face = model.entities.add_face([(0, 0, 0), (100, 0, 0), (100, 100, 0), (0, 100, 0)])
    face.front_material_id = front.id
    face.back_material_id = back.id
    face.front_uv = skppy.FaceUVProjection(transform=[1, 0, 0, 0, 1, 0, 0, 0, 1])
    face.back_uv = skppy.FaceUVProjection(transform=[0, 1, 0, -1, 0, 0, 100, 0, 1])
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "uv_back_mat"))
