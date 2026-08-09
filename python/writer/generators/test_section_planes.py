# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.section_planes.append(
        skppy.SectionPlane(
            id=1,
            plane=(0, 0, 1, -10),
            name="TestSection",
            symbol="TS",
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "section_planes"))
