# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.rendering_options = skppy.RenderingOptions(
        edge_display_mode=0,
        draw_ground=True,
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "rendering_options"))
