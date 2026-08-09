# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.model_view_axes = skppy.ModelViewAxes(
        origin=(10, 10, 10),
        x_axis=(1, 1, 0),
        y_axis=(-1, 1, 0),
        z_axis=(0, 0, 1),
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "axes"))
