# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import one_pixel_png, run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.watermark_manager = skppy.WatermarkManager(
        watermarks=[
            skppy.Watermark(
                name="Writer Overlay",
                image_data=one_pixel_png(),
                opacity=0.75,
                position=5,
            )
        ],
        serialized_count=1,
    )
    model.styles_registry = skppy.StylesRegistry(
        styles=[
            skppy.StyleDescriptor(
                guid=bytes.fromhex("a5d4a6b92d7e4ddea25d8618bca9cc5f"),
                display_name="Watermark Style",
                file_name="WatermarkStyle",
                watermark_reference_ids=[1],
            )
        ],
        active_style_ref=1,
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "watermarks"))
