# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import one_pixel_png, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    material = model.add_material("Writer Texture", color=skppy.Color(127, 127, 127))
    material.has_texture = True
    material.texture = skppy.Texture(
        filename="writer_texture.png",
        x_scale=2.0,
        y_scale=3.0,
        data=one_pixel_png(),
    )
    face = model.entities.add_face([(0, 0, 0), (2, 0, 0), (0, 3, 0)])
    face.front_material_id = material.id
    face.front_uv = skppy.FaceUVProjection(
        transform=[1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0],
        origin=(0.0, 0.0, 0.0),
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "texture_uv"))
