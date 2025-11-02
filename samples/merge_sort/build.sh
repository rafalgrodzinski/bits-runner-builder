#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"
BRB_PATH="${SCRIPT_DIR}/../../build/brb"

"${BRB_PATH}" "${SCRIPT_DIR}/io.brc" "${SCRIPT_DIR}/main.brc" &&
cc -o merge_sort io.o main.o