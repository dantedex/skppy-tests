# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.options_manager = skppy.OptionsManager(
        providers=[
            skppy.OptionsProvider(
                name="UnitsOptions",
                keys={
                    "LengthUnit": 2,
                    "LengthSnapEnabled": True,
                    "LengthSnapLength": 0.25,
                },
            )
        ]
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "options"))
