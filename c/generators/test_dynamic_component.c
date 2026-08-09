// SPDX-License-Identifier: MIT
#include "test_utils.h"
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/component_definition.h>
#include <SketchUpAPI/model/attribute_dictionary.h>
#include <SketchUpAPI/model/typed_value.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (!init_sketchup()) return 1;
    enum SUModelVersion ver = version_from_args(argc, argv);

    SUModelRef model = SU_INVALID;
    SUModelCreate(&model);

    // Dynamic Components are essentially attribute dictionaries
    SUComponentDefinitionRef def = SU_INVALID;
    SUComponentDefinitionCreate(&def);
    SUComponentDefinitionSetName(def, "DynamicComponent");
    
    SUAttributeDictionaryRef dict = SU_INVALID;
    SUAttributeDictionaryCreate(&dict, "dynamic_attributes");
    
    SUTypedValueRef val = SU_INVALID;
    SUTypedValueCreate(&val);
    SUTypedValueSetString(val, "100");
    SUAttributeDictionarySetValue(dict, "_lenx_nominal", val);
    
    SUTypedValueRelease(&val);
    
    SUEntityRef ent = SUComponentDefinitionToEntity(def);
    SUEntityAddAttributeDictionary(ent, dict);
    
    SUModelAddComponentDefinitions(model, 1, &def);

    save_model_with_version(model, "data/dynamic_component.skp", ver);

    SUModelRelease(&model);
    term_sketchup();
    return 0;
}
