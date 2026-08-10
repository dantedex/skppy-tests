# SPDX-License-Identifier: MIT
import os
import unittest

import skppy


FEATURE_MINIMUM_MAJOR = {
    "component_behavior": 4,
    "face_uv_projection": 4,
    "entity_attribute_dictionaries": 4,
    "component_instance_name": 5,
    "attribute_dictionaries": 7,
    "section_plane_name": 18,
    "layer_folders": 20,
    "layer_folder_membership": 21,
    "dimension_point_reference_instance_path": 21,
}


def model_format_major(model):
    """Return the file-format major version, or ``None`` when unavailable."""
    version = getattr(getattr(model, "header", None), "version_tuple", None)
    return version[0] if version else None


def supports_feature(model, feature):
    """Return whether the saved format can represent one generated feature."""
    minimum = FEATURE_MINIMUM_MAJOR[feature]
    major = model_format_major(model)
    return major is not None and major >= minimum


# ---------------------------------------------------------------------------
# Output directory resolution
# ---------------------------------------------------------------------------
# OUTPUT_DIR can be set via environment to redirect where generated .skp
# fixtures are read from.  When unset, falls back to the project root
# (parent of python/).
_OUTPUT_ROOT = os.environ.get("OUTPUT_DIR")
if _OUTPUT_ROOT:
    DATA_DIR = os.path.join(_OUTPUT_ROOT, "data")
else:
    # Default: project root is one level up from python/validation/
    _project_root = os.path.dirname(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    DATA_DIR = os.path.join(_project_root, "data")


class BaseSkpTest(unittest.TestCase):
    def load_model(self, filename):
        path = os.path.join(DATA_DIR, filename)
        if not os.path.exists(path):
            self.skipTest(f"{path} not found")
        return skppy.load(path)

    def supports_feature(self, model, feature):
        """Return whether the model's saved format supports *feature*."""
        return supports_feature(model, feature)

    def assert_restrictive(
        self,
        model,
        expected_entities=-1,
        expected_materials=0,
        expected_definitions=0,
        expected_layers=0,
        expected_scenes=0,
        expected_cameras=1,
        expected_attribute_dicts=None,
    ):
        """
        Check that the model contains exactly the expected number of elements.
        Set -1 to skip a check.
        """
        if expected_entities != -1:
            # Flattening entities for count comparison
            total_entities = (
                len(model.entities.vertices)
                + len(model.entities.edges)
                + len(model.entities.faces)
                + len(model.entities.component_instances)
                + len(model.entities.groups)
                + len(model.entities.images)
                + len(model.entities.curves)
                + len(model.entities.arc_curves)
                + len(model.entities.guide_points)
                + len(model.entities.guide_lines)
                + len(model.entities.section_planes)
            )
            self.assertEqual(
                total_entities,
                expected_entities,
                f"Expected {expected_entities} total entities, found {total_entities}",
            )

        if expected_materials != -1:
            self.assertEqual(
                len(model.materials),
                expected_materials,
                f"Expected {expected_materials} materials, found {len(model.materials)}",
            )

        if expected_definitions != -1:
            self.assertEqual(
                len(model.definitions),
                expected_definitions,
                f"Expected {expected_definitions} definitions, found {len(model.definitions)}",
            )

        if expected_layers != -1:
            # Most models have at least one layer (Layer0)
            self.assertEqual(
                len(model.layers),
                expected_layers,
                f"Expected {expected_layers} layers, found {len(model.layers)}",
            )

        if expected_scenes != -1:
            self.assertEqual(
                len(model.scenes),
                expected_scenes,
                f"Expected {expected_scenes} scenes, found {len(model.scenes)}",
            )

        if expected_cameras != -1:
            self.assertEqual(
                len(model.cameras),
                expected_cameras,
                f"Expected {expected_cameras} cameras, found {len(model.cameras)}",
            )

        if expected_attribute_dicts is None:
            # The SDK down-converter omits the model dictionary container in
            # SU3-SU6 files. SU7 introduced the archive representation used by
            # the generated fixtures; modern files retain the same semantic
            # default dictionary.
            expected_attribute_dicts = (
                1 if self.supports_feature(model, "attribute_dictionaries") else 0
            )
        if expected_attribute_dicts != -1:
            self.assertEqual(
                len(model.attribute_dictionaries),
                expected_attribute_dicts,
                f"Expected {expected_attribute_dicts} model attribute dicts, found {len(model.attribute_dictionaries)}",
            )

    def assert_entities_count(
        self,
        entities,
        vertices=0,
        edges=0,
        faces=0,
        instances=0,
        groups=0,
        images=0,
        curves=0,
        arc_curves=0,
        guide_points=0,
        guide_lines=0,
        section_planes=0,
    ):
        self.assertEqual(
            len(entities.vertices),
            vertices,
            f"Vertices count mismatch: expected {vertices}, found {len(entities.vertices)}",
        )
        self.assertEqual(
            len(entities.edges),
            edges,
            f"Edges count mismatch: expected {edges}, found {len(entities.edges)}",
        )
        self.assertEqual(
            len(entities.faces),
            faces,
            f"Faces count mismatch: expected {faces}, found {len(entities.faces)}",
        )
        self.assertEqual(
            len(entities.component_instances),
            instances,
            f"Instances count mismatch: expected {instances}, found {len(entities.component_instances)}",
        )
        self.assertEqual(
            len(entities.groups),
            groups,
            f"Groups count mismatch: expected {groups}, found {len(entities.groups)}",
        )
        self.assertEqual(
            len(entities.images),
            images,
            f"Images count mismatch: expected {images}, found {len(entities.images)}",
        )
        self.assertEqual(
            len(entities.curves),
            curves,
            f"Curves count mismatch: expected {curves}, found {len(entities.curves)}",
        )
        self.assertEqual(
            len(entities.arc_curves),
            arc_curves,
            f"Arc curves count mismatch: expected {arc_curves}, found {len(entities.arc_curves)}",
        )
        self.assertEqual(
            len(entities.guide_points),
            guide_points,
            f"Guide points count mismatch: expected {guide_points}, found {len(entities.guide_points)}",
        )
        self.assertEqual(
            len(entities.guide_lines),
            guide_lines,
            f"Guide lines count mismatch: expected {guide_lines}, found {len(entities.guide_lines)}",
        )
        self.assertEqual(
            len(entities.section_planes),
            section_planes,
            f"Section planes count mismatch: expected {section_planes}, found {len(entities.section_planes)}",
        )
