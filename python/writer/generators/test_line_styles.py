# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.line_styles.append(
        skppy.LineStyle(
            name="Writer Dash",
            dash_pattern="12.0, -6.0",
            stipple_scale=2.0,
            line_width_points=2.5,
            color=0xFF0A141E,
            mutability=True,
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "line_styles"))
