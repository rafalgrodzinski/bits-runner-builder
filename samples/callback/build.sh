#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

brb "${SCRIPT_DIR}/io.brc" "${SCRIPT_DIR}/utils.brc" "${SCRIPT_DIR}/main.brc" &&
cc -o callback io.o utils.o main.o