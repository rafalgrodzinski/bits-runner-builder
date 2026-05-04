#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

brb "${SCRIPT_DIR}/io.brc" "${SCRIPT_DIR}/stuff.brc" "${SCRIPT_DIR}/main.brc" &&
cc -o multi_module io.o stuff.o main.o