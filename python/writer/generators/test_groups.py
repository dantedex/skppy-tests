# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def _edge(entities) -> None:
    start = entities.add_vertex(0, 0, 0)
    end = entities.add_vertex(5, 0, 0)
    entities.add_edge(start, end)


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    face_definition, _ = model.add_group("Face Group")
    face_definition.entities.add_face([(0, 0, 0), (4, 0, 0), (4, 4, 0), (0, 4, 0)])

    inner_definition, inner_group = model.add_group("Inner Group")
    _edge(inner_definition.entities)
    outer_definition = model.add_definition("Outer Group Definition")
    outer_definition.definition_type = 1
    model.entities.groups.remove(inner_group)
    outer_definition.entities.groups.append(inner_group)
    model.entities.groups.append(
        skppy.Group(id=2, name="Outer Group", definition_id=outer_definition.id)
    )

    component = model.add_definition("GroupedComponent")
    _edge(component.entities)
    component_group_definition = model.add_definition("Component Group Definition")
    component_group_definition.definition_type = 1
    component_group_definition.entities.add_instance(component)
    model.entities.groups.append(
        skppy.Group(
            id=3,
            name="Component Group",
            definition_id=component_group_definition.id,
        )
    )
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "groups"))
