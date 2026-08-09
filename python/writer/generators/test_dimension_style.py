# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.dimension_style = skppy.DimensionStyle(
        font_ref=2,
        text_3d=True,
        always_readable=True,
        extension_offset=5,
        extension_overshoot=10,
        line_weight=2,
        arrow_type=3,
        arrow_size=12,
        highlight_non_associative=True,
        highlight_non_associative_color=0xFF00FF00,
        show_radial_diameter_prefix=True,
        hide_out_of_plane=True,
        hide_out_of_plane_value=0.6,
        hide_small=True,
        hide_small_value=10.0,
        color=0xFF404040,
        text_color=0xFF0A141E,
        text_position=1,
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "dimension_style"))
