# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.cameras.append(
        skppy.Camera(
            eye=skppy.Vector3D(100, 200, 300),
            target=skppy.Vector3D(10, 20, 30),
            up=skppy.Vector3D(0, 0, 1),
            fov=42.0,
            fov_is_height=False,
            name="WriterCamera",
            allow_clipping=False,
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "camera"))
