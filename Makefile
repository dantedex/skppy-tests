# Makefile for skppy-tests
# This Makefile cross-compiles test generators for Windows using MinGW.
#
# The tracked include/ and bin/ directories contain the public SketchUp C API
# headers, import libraries, and runtime DLLs used by the conformance suite.
# SDK_DIR may point at a different SDK root, which must contain:
#   include/SketchUpAPI/...   (headers)
#   bin/SketchUpAPI.lib       (import library)
#   bin/sketchup.lib          (import library)
#   bin/*.dll                 (runtime DLLs, copied next to the exes)
#
# Override on the command line, e.g.:
#   make generate SDK_DIR=/path/to/sketchup_sdk
#
# OUTPUT_DIR controls where generated .skp files and compiled binaries are
# written.  Defaults to the project root's conventional directories (bin/,
# data/).  Override to redirect everything, e.g.:
#   make generate OUTPUT_DIR=/tmp/skp_output

SDK_DIR ?= .

CC = x86_64-w64-mingw32-gcc
CFLAGS = -I$(SDK_DIR)/include -DSU_SUPPRESS_DEPRECATION_WARNINGS -O2
LDFLAGS = $(SDK_DIR)/bin/SketchUpAPI.lib $(SDK_DIR)/bin/sketchup.lib

GEN_DIR = c/generators
VALIDATOR_DIR = c/validators

# Output directory prefix.  When set, BIN_DIR and DATA_DIR are derived from it.
# When unset, they default to ./bin and ./data.
OUTPUT_DIR ?=

# Resolve output directories: if OUTPUT_DIR is set, use it; otherwise fall back
# to the legacy defaults (bin/, data/) at the project root.
BIN_DIR := $(if $(OUTPUT_DIR),$(OUTPUT_DIR)/bin,bin)
DATA_DIR := $(if $(OUTPUT_DIR),$(OUTPUT_DIR)/data,data)
GENERATOR_WORK_DIR := $(if $(OUTPUT_DIR),$(abspath $(OUTPUT_DIR)),$(CURDIR))
GENERATOR_BIN_DIR := $(abspath $(BIN_DIR))

# SketchUp file-format version to save as.  Override on the command line, e.g.:
#   make generate VERSION=SU8
#   make generate VERSION=SU2017
#   make generate VERSION=current
# When empty/unset, the generators save using the SDK default (current).
VERSION =

# Complete set of unique file formats exposed by SUModelVersion in the SDK.
# SUModelVersion_Current aliases SU2021 and is intentionally not duplicated.
VERSIONS ?= SU3 SU4 SU5 SU6 SU7 SU8 SU2013 SU2014 SU2015 SU2016 SU2017 SU2018 SU2019 SU2020 SU2021
MATRIX_DIR := $(if $(OUTPUT_DIR),$(OUTPUT_DIR)/matrix,matrix)
SKPPY_PATH ?= ../skppy
PYTHON ?= python3
MATRIX_JOBS ?= 1
SEMANTIC_VALIDATION ?= 0
WRITER_BIN_DIR := $(BIN_DIR)/writer
WRITER_DATA_DIR := $(DATA_DIR)/writer
LEGACY_WRITER_DATA_DIR := $(DATA_DIR)/writer_legacy
WRITER_GENERATOR_DIR := python/writer/generators
WRITER_GENERATORS := $(wildcard $(WRITER_GENERATOR_DIR)/test_*.py)
WRITER_GENERATOR_MODULES := $(patsubst %.py,%,$(subst /,.,$(patsubst python/%,%,$(WRITER_GENERATORS))))
WRITER_VALIDATOR_DIR := $(VALIDATOR_DIR)/writer
WRITER_VALIDATOR_SOURCES := $(filter-out $(WRITER_VALIDATOR_DIR)/test_utils.c,$(wildcard $(WRITER_VALIDATOR_DIR)/test_*.c))
WRITER_VALIDATORS := $(patsubst $(WRITER_VALIDATOR_DIR)/%.c,$(WRITER_BIN_DIR)/%.exe,$(WRITER_VALIDATOR_SOURCES))
BLENDER_VALIDATOR_DIR := $(VALIDATOR_DIR)/blender
BLENDER_VALIDATOR_SOURCES := $(wildcard $(BLENDER_VALIDATOR_DIR)/test_*.c)
BLENDER_VALIDATORS := $(patsubst $(BLENDER_VALIDATOR_DIR)/%.c,$(BIN_DIR)/blender/%.exe,$(BLENDER_VALIDATOR_SOURCES))
BLENDER_EXPORT_DIR ?= /tmp/skppy-blender-validation

SOURCES = $(wildcard $(GEN_DIR)/test_*.c)
# Exclude utils from direct compilation to exe
EXE_SOURCES = $(filter-out $(GEN_DIR)/test_utils.c, $(SOURCES))
EXECUTABLES = $(patsubst $(GEN_DIR)/%.c, $(BIN_DIR)/%.exe, $(EXE_SOURCES))

all: $(BIN_DIR) $(DATA_DIR) $(EXECUTABLES)

$(BIN_DIR)/%.exe: $(GEN_DIR)/%.c $(GEN_DIR)/test_utils.c
	@echo "[build] $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(DATA_DIR):
	mkdir -p $(DATA_DIR)

# Build the -version argument passed to every generator.  When VERSION is
# empty, no flag is passed and the generator saves with the SDK default.
VERSION_FLAG = $(if $(VERSION),-version $(VERSION),)

generate: all
	@for exe in $(EXECUTABLES); do \
		echo "[generate] $$exe $(VERSION_FLAG)"; \
		cp $(SDK_DIR)/bin/*.dll $(BIN_DIR)/; \
		(cd "$(GENERATOR_WORK_DIR)" && \
			WINEDEBUG=-all wine "$(GENERATOR_BIN_DIR)/$${exe##*/}" $(VERSION_FLAG)) \
			|| echo "FAILED: $$exe"; \
	done

# Generate every SDK file format into an isolated directory and smoke-import
# every resulting SKP with the requested local skppy checkout.
matrix: all
	cp $(SDK_DIR)/bin/*.dll $(BIN_DIR)/
	$(PYTHON) python/version_matrix.py \
		--bin-dir $(BIN_DIR) \
		--output-dir $(MATRIX_DIR) \
		--skppy-path $(SKPPY_PATH) \
		--resources-dir $(SDK_DIR)/resources \
		--jobs $(MATRIX_JOBS) \
		--clean \
		$(if $(filter 1 true yes,$(SEMANTIC_VALIDATION)),--semantic-validation) \
		$(VERSIONS)

# Release validation adds version-aware semantic assertions to the smoke matrix.
release-matrix: SEMANTIC_VALIDATION=1
release-matrix: matrix

# Exercise the inverse integration path: Python/skppy writes each fixture and
# an independent C process opens and inspects it through the public SDK.
$(WRITER_BIN_DIR)/%.exe: $(WRITER_VALIDATOR_DIR)/%.c $(WRITER_VALIDATOR_DIR)/test_utils.c
	mkdir -p $(WRITER_BIN_DIR)
	@echo "[build] $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

writer-validation: $(WRITER_VALIDATORS)
	mkdir -p $(WRITER_DATA_DIR)
	@for generator in $(WRITER_GENERATOR_MODULES); do \
		PYTHONPATH=python $(PYTHON) -m $$generator \
			--skppy-path $(SKPPY_PATH) \
			--output-dir $(WRITER_DATA_DIR) || exit 1; \
	done
	cp $(SDK_DIR)/bin/*.dll $(WRITER_BIN_DIR)/
	@writer_data=$$(winepath -w "$(abspath $(WRITER_DATA_DIR))"); \
		for validator in $(WRITER_VALIDATORS); do \
			WINEDEBUG=-all wine $$validator "$$writer_data" || exit 1; \
	done

legacy-writer-validation: $(WRITER_VALIDATORS)
	mkdir -p $(LEGACY_WRITER_DATA_DIR)
	@for generator in $(WRITER_GENERATOR_MODULES); do \
		PYTHONPATH=python $(PYTHON) -m $$generator \
			--skppy-path $(SKPPY_PATH) \
			--output-dir $(LEGACY_WRITER_DATA_DIR) \
			--format sketchup_2017 || exit 1; \
	done
	cp $(SDK_DIR)/bin/*.dll $(WRITER_BIN_DIR)/
	@writer_data=$$(winepath -w "$(abspath $(LEGACY_WRITER_DATA_DIR))"); \
		for validator in $(WRITER_VALIDATORS); do \
			WINEDEBUG=-all wine $$validator "$$writer_data" || exit 1; \
		done

$(BIN_DIR)/blender/%.exe: $(BLENDER_VALIDATOR_DIR)/%.c $(WRITER_VALIDATOR_DIR)/test_utils.c
	mkdir -p $(BIN_DIR)/blender
	@echo "[build] $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

blender-validation: $(BLENDER_VALIDATORS)
	cp $(SDK_DIR)/bin/*.dll $(BIN_DIR)/blender/
	@blender_data=$$(winepath -w "$(abspath $(BLENDER_EXPORT_DIR))"); \
		for validator in $(BLENDER_VALIDATORS); do \
			WINEDEBUG=-all wine $$validator "$$blender_data" || exit 1; \
		done

clean:
	rm -rf $(BIN_DIR) $(DATA_DIR)

.PHONY: all generate matrix release-matrix writer-validation legacy-writer-validation blender-validation clean
