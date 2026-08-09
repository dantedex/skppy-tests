# SPDX-License-Identifier: MIT
"""Shared CLI and fixture helpers for independent writer generators."""

from __future__ import annotations

import argparse
import base64
import math
import struct
import sys
import zlib
from collections.abc import Callable, Iterable, Sequence
from pathlib import Path
from types import ModuleType


def import_skppy(checkout: Path) -> ModuleType:
    """Import skppy from the checkout explicitly selected by the test."""
    checkout = checkout.resolve()
    if not (checkout / "skppy" / "__init__.py").is_file():
        raise SystemExit(f"not a skppy checkout: {checkout}")
    sys.path.insert(0, str(checkout))
    import skppy

    return skppy


def run(generator: Callable[[ModuleType, Path], None], fixture_name: str) -> int:
    """Run one fixture generator with the shared command-line contract."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--skppy-path", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--format", choices=("modern", "sketchup_2017"))
    args = parser.parse_args()
    output_dir = args.output_dir.resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    destination = output_dir / f"{fixture_name}.skp"
    skppy = import_skppy(args.skppy_path)
    if args.format is not None:
        original_save = skppy.save

        def save(
            model,
            filepath,
            *,
            header=None,
            format="modern",
            export_vray_materials=False,
        ):
            return original_save(
                model,
                filepath,
                header=header,
                format=args.format,
                export_vray_materials=export_vray_materials,
            )

        skppy.save = save
    generator(skppy, destination)
    print(f"[writer-generate] {destination}")
    return 0


def one_pixel_png() -> bytes:
    """Return a valid one-pixel RGBA PNG without external dependencies."""

    def chunk(kind: bytes, payload: bytes) -> bytes:
        body = kind + payload
        return (
            struct.pack(">I", len(payload)) + body + struct.pack(">I", zlib.crc32(body))
        )

    header = struct.pack(">IIBBBBB", 1, 1, 8, 6, 0, 0, 0)
    pixels = zlib.compress(bytes((0, 127, 127, 127, 255)))
    return (
        b"\x89PNG\r\n\x1a\n"
        + chunk(b"IHDR", header)
        + chunk(b"IDAT", pixels)
        + chunk(b"IEND", b"")
    )


def one_pixel_jpeg() -> bytes:
    """Return a valid one-pixel RGB JPEG without external dependencies."""
    return base64.b64decode(
        "/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAMCAgICAgMCAgIDAwMDBAYEBAQEBAgG"
        "BgUGCQgKCgkICQkKDA8MCgsOCwkJDRENDg8QEBEQCgwSExIQEw8QEBD/wAALCAAB"
        "AAEBAREA/8QAFAABAAAAAAAAAAAAAAAAAAAAAv/EABQQAQAAAAAAAAAAAAAAAAAA"
        "AAD/2gAIAQEAAD8AD//Z"
    )


def add_holes(
    entities: object, holes: Iterable[Sequence[tuple[float, float, float]]]
) -> None:
    """Attach hole loops using only skppy's public entity graph."""
    face = entities.faces[0]
    for points in holes:
        hole_face = entities.add_face(list(reversed(points)))
        face.inner_loops.append(hole_face.outer_loop)
        entities.faces.pop()


def circle_points(
    radius: float,
    *,
    z: float = 0.0,
    segments: int = 24,
) -> list[tuple[float, float, float]]:
    """Return counter-clockwise points for a polygonal circle."""
    return [
        (
            radius * math.cos(2.0 * math.pi * index / segments),
            radius * math.sin(2.0 * math.pi * index / segments),
            z,
        )
        for index in range(segments)
    ]


def add_indexed_faces(
    skppy: ModuleType,
    entities: object,
    points: Sequence[tuple[float, float, float]],
    faces: Sequence[Sequence[int]],
) -> None:
    """Build shared-vertex/shared-edge polygon topology from index loops."""
    vertices = [
        skppy.Vertex(index + 1, skppy.Vector3D(*point))
        for index, point in enumerate(points)
    ]
    entities.vertices.extend(vertices)
    edge_pairs: list[tuple[int, int]] = []
    edge_index: dict[tuple[int, int], int] = {}
    for face in faces:
        for index, start in enumerate(face):
            end = face[(index + 1) % len(face)]
            key = tuple(sorted((start, end)))
            if key not in edge_index:
                edge_index[key] = len(edge_pairs)
                edge_pairs.append((start, end))
    first_edge_id = len(vertices) + 1
    entities.edges.extend(
        skppy.Edge(first_edge_id + index, start + 1, end + 1)
        for index, (start, end) in enumerate(edge_pairs)
    )
    first_face_id = first_edge_id + len(edge_pairs)
    for face_index, indices in enumerate(faces):
        uses = []
        for index, start in enumerate(indices):
            end = indices[(index + 1) % len(indices)]
            stored_index = edge_index[tuple(sorted((start, end)))]
            uses.append(
                skppy.EdgeUse(
                    first_edge_id + stored_index,
                    edge_pairs[stored_index] != (start, end),
                )
            )
        plane = _plane_from_indices(points, indices)
        entities.faces.append(
            skppy.Face(
                id=first_face_id + face_index,
                plane=plane,
                outer_loop=skppy.Loop(uses, is_outer=True),
                inner_loops=[],
            )
        )


def _plane_from_indices(
    points: Sequence[tuple[float, float, float]], indices: Sequence[int]
) -> tuple[float, float, float, float]:
    x0, y0, z0 = points[indices[0]]
    x1, y1, z1 = points[indices[1]]
    x2, y2, z2 = points[indices[2]]
    ux, uy, uz = x1 - x0, y1 - y0, z1 - z0
    vx, vy, vz = x2 - x0, y2 - y0, z2 - z0
    nx, ny, nz = uy * vz - uz * vy, uz * vx - ux * vz, ux * vy - uy * vx
    length = math.sqrt(nx * nx + ny * ny + nz * nz)
    nx, ny, nz = nx / length, ny / length, nz / length
    return nx, ny, nz, -(nx * x0 + ny * y0 + nz * z0)
