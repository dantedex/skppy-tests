# SPDX-License-Identifier: MIT
import math
from pathlib import Path

from .common import add_indexed_faces, run


def _edge_for(entities, start: int, end: int):
    vertex_ids = {start + 1, end + 1}
    return next(
        edge
        for edge in entities.edges
        if {edge.start_vertex_id, edge.end_vertex_id} == vertex_ids
    )


def _loop(skppy, entities, indices):
    uses = []
    for index, start in enumerate(indices):
        end = indices[(index + 1) % len(indices)]
        edge = _edge_for(entities, start, end)
        uses.append(
            skppy.EdgeUse(
                edge.id,
                (edge.start_vertex_id, edge.end_vertex_id) != (start + 1, end + 1),
            )
        )
    return skppy.Loop(uses, is_outer=False)


def _ring_edges(entities, indices):
    edges = [
        _edge_for(entities, start, indices[(index + 1) % len(indices)])
        for index, start in enumerate(indices)
    ]
    for index, edge in enumerate(edges):
        expected = (indices[index] + 1, indices[(index + 1) % len(indices)] + 1)
        if (edge.start_vertex_id, edge.end_vertex_id) == expected:
            continue
        edge.start_vertex_id, edge.end_vertex_id = (
            edge.end_vertex_id,
            edge.start_vertex_id,
        )
        for face in entities.faces:
            for loop in (face.outer_loop, *face.inner_loops):
                for edge_use in loop.edge_uses:
                    if edge_use.edge_id == edge.id:
                        edge_use.reversed = not edge_use.reversed
    return [edge.id for edge in edges]


def generate(skppy, destination: Path) -> None:
    segments = 24
    outer_radius = 39.37007874015748
    inner_radius = 19.758832678565383
    height = 39.37007874015748
    points = []
    for radius, z in (
        (outer_radius, 0.0),
        (outer_radius, height),
        (inner_radius, 0.0),
        (inner_radius, height),
    ):
        points.extend(
            (
                radius * math.cos(2.0 * math.pi * index / segments),
                radius * math.sin(2.0 * math.pi * index / segments),
                z,
            )
            for index in range(segments)
        )

    bottom_outer = list(range(0, 24))
    top_outer = list(range(24, 48))
    bottom_inner = list(range(48, 72))
    top_inner = list(range(72, 96))
    faces = []
    for index in range(segments):
        following = (index + 1) % segments
        faces.append([index, following, following + 24, index + 24])
    for index in range(segments):
        following = (index + 1) % segments
        faces.append([index + 48, index + 72, following + 72, following + 48])
    faces.extend([list(reversed(bottom_outer)), top_outer])

    model = skppy.new_model()
    add_indexed_faces(skppy, model.entities, points, faces)
    model.entities.faces[-2].inner_loops.append(
        _loop(skppy, model.entities, bottom_inner)
    )
    model.entities.faces[-1].inner_loops.append(
        _loop(skppy, model.entities, list(reversed(top_inner)))
    )
    next_id = (
        max(
            entity.id
            for family in (
                model.entities.vertices,
                model.entities.edges,
                model.entities.faces,
            )
            for entity in family
        )
        + 1
    )
    for indices, center, radius in (
        (bottom_outer, (0, 0, 0), outer_radius),
        (top_outer, (0, 0, height), outer_radius),
        (bottom_inner, (0, 0, 0), inner_radius),
        (top_inner, (0, 0, height), inner_radius),
    ):
        edge_ids = _ring_edges(model.entities, indices)
        arc = skppy.ArcCurve(
            id=next_id,
            edge_ids=edge_ids,
            center=center,
            normal=(0, 0, 1),
            radius=radius,
            start_angle=0.0,
            end_angle=2.0 * math.pi,
        )
        next_id += 1
        model.entities.arc_curves.append(arc)
        for edge_id in edge_ids:
            edge = next(edge for edge in model.entities.edges if edge.id == edge_id)
            edge.curve_id = arc.id
            edge.flags |= 6
    skppy.save(model, destination)


if __name__ == "__main__":
    raise SystemExit(run(generate, "cylinder"))
