# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    skppy.save(skppy.new_model(), destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "empty"))
