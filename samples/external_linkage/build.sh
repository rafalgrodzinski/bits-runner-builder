#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

brb "${SCRIPT_DIR}/stuff.brc" &&
brb "${SCRIPT_DIR}/main.brc" "${SCRIPT_DIR}/io.brc" &&
cc -o external_linkage main.o io.o stuff.o