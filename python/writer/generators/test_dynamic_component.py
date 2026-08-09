# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    definition = model.add_definition("DynamicComponent")
    model.attribute_dictionaries_by_object_id[definition.id] = [
        skppy.AttributeDictionary(
            name="dynamic_attributes",
            entries=[
                skppy.AttributeDictionaryEntry(
                    key="_lenx_nominal",
                    value_type=3,
                    string_value="100",
                )
            ],
        )
    ]
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "dynamic_component"))
