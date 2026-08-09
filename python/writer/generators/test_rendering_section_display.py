# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def _save_options(skppy, destination: Path, mask: int) -> None:
    model = skppy.new_model()
    model.rendering_options = skppy.RenderingOptions(section_display_mode=mask)
    skppy.save(model, destination)


def generate(skppy, destination: Path) -> None:
    output_dir = destination.parent
    _save_options(skppy, output_dir / "rendering_section_planes_false.skp", 2)
    _save_options(skppy, output_dir / "rendering_section_planes_true.skp", 3)
    _save_options(skppy, output_dir / "rendering_section_cuts_false.skp", 1)
    _save_options(skppy, output_dir / "rendering_section_cuts_true.skp", 3)


if __name__ == "__main__":
    raise SystemExit(run(generate, "rendering_section_display"))
