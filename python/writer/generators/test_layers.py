# SPDX-License-Identifier: MIT
from pathlib import Path

from .common import run


def generate(skppy, destination: Path) -> None:
    model = skppy.new_model()
    walls = model.add_layer("Writer Walls")
    roof = model.add_layer("Writer Roof", visible=False)
    walls.material = skppy.Material(
        id=100, name="Writer Layer Color", color=skppy.Color(10, 20, 30)
    )
    model.active_layer_id = roof.id
    face = model.entities.add_face([(0, 0, 0), (2, 0, 0), (0, 2, 0)])
    face.layer_id = walls.id
    model.entities.edges[0].layer_id = roof.id
    model.layer_folders = [
        skppy.LayerFolder(
            name="Writer Building",
            child_layer_ids=[walls.id],
            child_folders=[
                skppy.LayerFolder(name="Writer Upper", child_layer_ids=[roof.id])
            ],
        )
    ]
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "layers"))
