#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"
BRB_PATH="${SCRIPT_DIR}/../../build/brb"

"${BRB_PATH}" "${SCRIPT_DIR}/console.brc" "${SCRIPT_DIR}/sample_modules.brc"
cc -o sample_modules console.o app.o