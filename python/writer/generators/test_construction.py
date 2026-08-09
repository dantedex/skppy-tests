# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    layer = model.add_layer("Writer Construction")
    model.entities.guide_lines.append(
        skppy.GuideLine(
            id=1,
            point=(1, 2, 3),
            direction=(0, 1, 0),
            stipple_pattern=0xFFFF,
            start_parameter=0,
            end_parameter=5,
            layer_id=layer.id,
        )
    )
    model.entities.guide_points.append(
        skppy.GuidePoint(id=2, position=(4, 5, 6), layer_id=layer.id)
    )
    model.entities.section_planes.append(
        skppy.SectionPlane(
            id=3,
            plane=(1, 0, 0, -7),
            name="Writer Section",
            symbol="WS",
            layer_id=layer.id,
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "construction"))
