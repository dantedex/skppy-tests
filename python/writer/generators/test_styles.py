# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.styles_registry = skppy.StylesRegistry(
        styles=[
            skppy.StyleDescriptor(
                guid=bytes.fromhex("126adcd9afed4f4790451cc3e2291ec6"),
                display_name="Writer Style",
                file_name="WriterStyle",
            )
        ],
        active_style_ref=1,
        selected_style_dirty=True,
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "styles"))
