# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    camera = skppy.Camera(
        eye=skppy.Vector3D(100, 100, 100),
        target=skppy.Vector3D(0, 0, 0),
        up=skppy.Vector3D(0, 0, 1),
    )
    model.scenes.append(skppy.Scene(id=1, name="TestScene", flags=1, camera=camera))
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "scenes"))
