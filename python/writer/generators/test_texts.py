# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.texts.append(
        skppy.Text(
            id=1,
            text="Writer text",
            anchor=skppy.PointReference(kind=1, position=skppy.Vector3D(1.0, 2.0, 3.0)),
            font_id=2,
            screen_position=skppy.Vector2D(0.0, 0.0),
            leader_vector=skppy.Vector3D(0.0, 0.0, 1.0),
            view_direction=skppy.Vector3D(0.0, 0.0, 1.0),
            leader_type=0,
            line_weight=1,
            arrow_type=2,
            display_leader=False,
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "texts"))
