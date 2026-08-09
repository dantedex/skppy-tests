# SPDX-License-Identifier: MIT
import uuid
from pathlib import Path

from .common import one_pixel_png, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    material = model.add_material("_auto_", color=skppy.Color(0, 0, 0))
    material.texture = skppy.Texture(
        filename="writer-image.png",
        x_scale=512.0,
        y_scale=512.0,
        data=one_pixel_png(),
    )
    material.has_texture = True
    definition = model.add_definition("brick_wall_21_basecolor_image")
    definition.definition_type = 2
    face = definition.entities.add_face(
        [(0, 0, 0), (512, 0, 0), (512, 512, 0), (0, 512, 0)]
    )
    face.front_material_id = material.id
    face.back_material_id = material.id
    model.entities.images.append(
        skppy.Image(
            id=1,
            guid=uuid.uuid4().bytes,
            definition_id=definition.id,
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "image"))
