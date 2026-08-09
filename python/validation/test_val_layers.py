# SPDX-License-Identifier: MIT
import unittest

from common import BaseSkpTest


def _collect_folder_names(folders, acc=None):
    """Recursively collect all folder names from a folder tree."""
    if acc is None:
        acc = []
    for folder in folders:
        acc.append(folder.name)
        _collect_folder_names(folder.child_folders, acc)
    return acc


def _find_folder(folders, name):
    """Return the named folder regardless of an implicit root node."""
    for folder in folders:
        if folder.name == name:
            return folder
        found = _find_folder(folder.child_folders, name)
        if found is not None:
            return found
    return None


class TestSkpLayers(BaseSkpTest):
    def test_layers_model(self):
        model = self.load_model("layers.skp")

        # 3 layers (Layer0 and the two generated layers)
        self.assert_restrictive(model, expected_layers=3)

        layer_names = [layer.name for layer in model.layers]
        self.assertIn("Layer0", layer_names)
        self.assertIn("TestLayer", layer_names)
        self.assertIn("SecondLayer", layer_names)

        # SketchUp nests user-created folders inside a default root folder
        # (which has an empty name), so search the folder tree recursively.
        folder_names = _collect_folder_names(model.layer_folders)
        if self.supports_feature(model, "layer_folders"):
            self.assertIn("TestFolder", folder_names)
            test_folder = _find_folder(model.layer_folders, "TestFolder")
            self.assertIsNotNone(test_folder)
            if self.supports_feature(model, "layer_folder_membership"):
                generated_ids = {
                    layer.id
                    for layer in model.layers
                    if layer.name in {"TestLayer", "SecondLayer"}
                }
                self.assertEqual(set(test_folder.child_layer_ids), generated_ids)
            else:
                self.assertEqual(test_folder.child_layer_ids, [])
        else:
            self.assertEqual(folder_names, [])


if __name__ == "__main__":
    unittest.main()
