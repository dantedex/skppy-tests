# SPDX-License-Identifier: MIT
"""Generate post-2017 metadata inside a valid SU2017 attribute extension."""

from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    material = model.add_material("ExtensionSteel")
    material.metallic = 0.8
    material.roughness = 0.2
    layer = model.add_layer("ExtensionLayer")
    model.layer_folders = [
        skppy.LayerFolder(
            name="ExtensionFolder",
            child_layer_ids=[layer.id],
        ),
    ]
    model.line_styles = [
        skppy.LineStyle(
            name="ExtensionDash",
            dash_pattern="3,-2",
        ),
    ]
    environment = skppy.EnvironmentEntry(
        id=7,
        name="ExtensionStudio",
        image_filename="studio.hdr",
        image_data=b"HDR",
    )
    model.environment_data = skppy.EnvironmentData(environment, [environment])
    start = model.entities.add_vertex(0, 0, 0)
    end = model.entities.add_vertex(1, 0, 0)
    edge = model.entities.add_edge(start, end)
    edge.curve_id = 10
    model.entities.arc_curves = [
        skppy.ArcCurve(id=10, edge_ids=[edge.id], raw_arc_payload=bytes(range(128))),
    ]
    model.entities.section_planes = [
        skppy.SectionPlane(id=20, name="ExtensionCut", symbol="EC"),
    ]
    definition = model.add_definition("ExtensionPacked")
    definition.packed_payload = b"PACKED"
    model.shadow_info = skppy.ShadowInfo(edges_cast_shadows=True)
    skppy.save(model, destination, format="sketchup_2017")


if __name__ == "__main__":
    raise SystemExit(run(generate, "legacy_extensions"))
