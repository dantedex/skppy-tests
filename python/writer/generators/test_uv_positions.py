# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import one_pixel_png, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    material = model.add_material(
        "UVConformanceMaterial", color=skppy.Color(160, 160, 160)
    )
    material.has_texture = True
    material.texture = skppy.Texture(
        filename="uv_conformance.png",
        x_scale=1.0,
        y_scale=1.0,
        data=one_pixel_png(),
    )

    faces = (
        (
            [(0, 0, 0), (20, 0, 0), (20, 20, 0), (0, 20, 0)],
            [10, 0, 0, 0, 20, 0, 0, 0, 1],
        ),
        (
            [(30, 0, 0), (50, 0, 0), (50, 20, 0), (30, 20, 0)],
            [0, 20, 0, -20, 0, 0, 50, 0, 1],
        ),
        (
            [(60, 0, 0), (80, 0, 0), (80, 20, 0), (60, 20, 0)],
            [-20, 0, 0, 0, 20, 0, 80, 0, 1],
        ),
        (
            [(90, 0, 0), (110, 0, 0), (110, 0, 20), (90, 0, 20)],
            [20, 0, 0, 0, 20, 0, 90, 0, 1],
        ),
    )
    for points, transform in faces:
        face = model.entities.add_face(points)
        face.front_material_id = material.id
        face.front_uv = skppy.FaceUVProjection(transform=transform)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "uv_positions"))
