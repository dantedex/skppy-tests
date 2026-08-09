# SPDX-License-Identifier: MIT
"""Generate a legacy scene containing rendering and style snapshots."""

from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.rendering_options = skppy.RenderingOptions(draw_ground=True)
    model.styles_registry = skppy.StylesRegistry(
        styles=[
            skppy.StyleDescriptor(
                guid=bytes.fromhex("126adcd9afed4f4790451cc3e2291ec6"),
                display_name="Scene Style",
                file_name="SceneStyle",
            ),
        ],
        active_style_ref=1,
    )
    model.scenes.append(
        skppy.Scene(
            id=1,
            name="LegacySnapshots",
            flags=2,
            style_reference=1,
        ),
    )
    skppy.save(model, destination, format="sketchup_2017")


if __name__ == "__main__":
    raise SystemExit(run(generate, "legacy_scene_snapshots"))
