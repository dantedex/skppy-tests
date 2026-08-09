# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.shadow_info = skppy.ShadowInfo(latitude=45.0, longitude=-120.0)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "georeference"))
