# SPDX-License-Identifier: MIT
"""Generate one material carrying skppy-authored V-Ray metadata."""

from pathlib import Path

from .common import one_pixel_png, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    material = model.add_material(
        "V-Ray Paint",
        color=skppy.Color(128, 64, 0),
        alpha=0.75,
        metallic=0.8,
        roughness=0.25,
    )
    material.has_texture = True
    material.texture = skppy.Texture(filename="vray-paint.png", data=one_pixel_png())
    face = model.entities.add_face([(0, 0, 0), (10, 0, 0), (0, 10, 0)])
    face.front_material_id = material.id
    skppy.save(model, destination, export_vray_materials=True)


if __name__ == "__main__":
    raise SystemExit(run(generate, "vray_materials"))
