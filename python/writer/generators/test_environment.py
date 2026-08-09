# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import one_pixel_jpeg, run


def generate(skppy, destination: Path) -> None:
    image_path = Path("resources/studio.exr")
    if not image_path.is_file():
        raise FileNotFoundError(f"environment resource not found: {image_path}")

    model = skppy.new_model()
    environment = skppy.EnvironmentEntry(
        id=1,
        name="StudioEnvironment",
        image_filename=image_path.name,
        image_data=image_path.read_bytes(),
        thumbnail_data=one_pixel_jpeg(),
        use_as_skydome=True,
        use_for_reflections=True,
    )
    reflection = skppy.EnvironmentEntry(
        id=2,
        name="ReflectionEnvironment",
        image_filename=image_path.name,
        image_data=image_path.read_bytes(),
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
