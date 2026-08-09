# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def _dictionary(skppy, name="TestData", key="Message", value="Hello SketchUp"):
    return skppy.AttributeDictionary(
        name=name,
        entries=[
            skppy.AttributeDictionaryEntry(
                key=key,
                value_type=3,
                string_value=value,
            )
        ],
    )


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.entities.vertices.extend(
        (
            skppy.Vertex(1, skppy.Vector3D(0, 0, 0)),
            skppy.Vertex(2, skppy.Vector3D(10, 10, 10)),
        )
    )
    model.entities.edges.append(skppy.Edge(3, 1, 2))
    model.entities.attribute_dictionaries_by_entity_id[3] = [_dictionary(skppy)]
    model.entities.attribute_dictionaries_by_entity_id[1] = [
        _dictionary(skppy, "VertexData", "Role", "start")
    ]
    model.attribute_dictionaries = [_dictionary(skppy, "ModelData", "Author", "skppy")]
    material = model.add_material("AttributedMaterial")
    layer = model.add_layer("AttributedLayer")
    model.attribute_dictionaries_by_object_id[material.id] = [
        _dictionary(skppy, "MaterialData", "Kind", "paint")
    ]
    model.attribute_dictionaries_by_object_id[layer.id] = [
        _dictionary(skppy, "LayerData", "Discipline", "architecture")
    ]
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "attributes"))
