# SPDX-License-Identifier: MIT
from pathlib import Path
import math

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    model.fonts = [skppy.Font("Arial"), skppy.Font("Courier New", point_size=14)]
    ink = model.add_material("Dimension Ink", color=skppy.Color(0, 0, 0))
    reference_definition = model.add_definition("Dimension Reference")
    local_start = reference_definition.entities.add_vertex(0.0, 0.0, 0.0)
    local_end = reference_definition.entities.add_vertex(4.0, 0.0, 0.0)
    nested_edge = reference_definition.entities.add_edge(local_start, local_end)
    reference_instance = model.entities.add_instance(
        reference_definition,
        skppy.Transform.from_translation(1.0, 2.0, 3.0),
    )
    root_start = model.entities.add_vertex(1.0, 2.0, 3.0)
    root_end = model.entities.add_vertex(5.0, 2.0, 3.0)
    reference_edge = model.entities.add_edge(root_start, root_end)
    arc = model.entities.add_arc_curve(
        center=(0.0, 0.0, 0.0),
        normal=(0.0, 0.0, 1.0),
        radius=10.0,
        start_angle=0.0,
        end_angle=math.pi / 2.0,
        segments=12,
    )
    linear = skppy.LinearDimension(
        id=arc.id + 1,
        text="Writer length",
        arrow_type=3,
        start=skppy.PointReference(
            kind=5,
            position=skppy.Vector3D(0.25, 0.0, 0.0),
            entity_id=nested_edge.id,
            instance_path_ids=[reference_instance.id],
        ),
        end=skppy.PointReference(
            kind=5,
            position=skppy.Vector3D(1.0, 0.0, 0.0),
            entity_id=reference_edge.id,
        ),
        direction=skppy.Vector3D(0.0, 0.0, 1.0),
        render_direction=skppy.Vector3D(1.0, 0.0, 0.0),
        alignment=1,
    )
    linear.drawing.casts_shadows = False
    model.entities.linear_dimensions.append(linear)
    radial = skppy.RadialDimension(
        id=arc.id + 2,
        text="Writer radius",
        font=model.fonts[1],
        arrow_type=3,
        target_entity_id=arc.edge_ids[0],
        parameter=math.pi / 4.0,
        radius_ratio=1.5,
        is_diameter=True,
    )
    radial.drawing.material_id = ink.id
    radial.drawing.receives_shadows = False
    model.entities.radial_dimensions.append(radial)
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "dimensions"))
