# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import one_pixel_jpeg, run


ENVIRONMENT_DATA = b"#?RADIANCE\nFORMAT=32-bit_rle_rgbe\n\n-Y 1 +X 1\n\x80\x80\x80\x80"


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    environment = skppy.EnvironmentEntry(
        id=1,
        name="StudioEnvironment",
        image_filename="studio.hdr",
        image_data=ENVIRONMENT_DATA,
        thumbnail_data=one_pixel_jpeg(),
        use_as_skydome=True,
        use_for_reflections=True,
    )
    reflection = skppy.EnvironmentEntry(
        id=2,
        name="ReflectionEnvironment",
        image_filename="studio.hdr",
        image_data=ENVIRONMENT_DATA,
        thumbnail_data=one_pixel_jpeg(),
        use_as_skydome=False,
        use_for_reflections=True,
    )
    model.environment_data = skppy.EnvironmentData(
        selected=environment,
        entries=[environment, reflection],
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "environment"))
