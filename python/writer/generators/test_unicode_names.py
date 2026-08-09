# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.add_layer("Camada 東京")
    model.add_material("Material Café", color=skppy.Color(64, 128, 192))
    definition = model.add_definition(
        "Componente Ω",
        description="Descrição UTF-8",
    )
    model.entities.add_instance(definition, name="Instância ☃")
    model.scenes.append(
        skppy.Scene(
            id=1,
            name="Cena São Paulo",
            description="Visão principal",
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "unicode_names"))
