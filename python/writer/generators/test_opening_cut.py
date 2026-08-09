# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    definition = model.add_definition("OpeningComponent")
    definition.behavior_cuts_opening = True
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "opening_cut"))
