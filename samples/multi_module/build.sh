#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

brb "${SCRIPT_DIR}/main.brc" "${SCRIPT_DIR}/io.brc" "${SCRIPT_DIR}/stuff.brc" &&
cc -o multi_module main.o io.o stuff.o