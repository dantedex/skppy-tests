# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import one_pixel_png, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    image = skppy.PageBackgroundImage(
        path="writer-match.png",
        image_data=one_pixel_png(),
        width=1,
        height=1,
        visible=True,
        opacity=0.75,
        image_source=1,
    )
    camera = skppy.Camera(
        eye=skppy.Vector3D(10, 10, 10),
        target=skppy.Vector3D(0, 0, 0),
        up=skppy.Vector3D(0, 0, 1),
    )
    model.background_image = image
    model.scenes.append(
        skppy.Scene(
            id=1,
            name="Writer Match Photo",
            flags=1,
            camera=camera,
            background_image=image,
            display_background_image=True,
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "background_images"))
