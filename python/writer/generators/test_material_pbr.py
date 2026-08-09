# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.add_material(
        "PBRMaterial",
        color=skppy.Color(180, 180, 200),
        metallic=0.8,
        roughness=0.25,
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "material_pbr"))
