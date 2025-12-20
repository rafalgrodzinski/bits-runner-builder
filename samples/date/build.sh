#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"
BRB_PATH="${SCRIPT_DIR}/../../build/brb"

"${BRB_PATH}" "${SCRIPT_DIR}/main.brc" "${SCRIPT_DIR}/../../lib/b/String.brc" "${SCRIPT_DIR}/../../lib/b/Date.brc" "${SCRIPT_DIR}/../../lib/b/BSys.brc" &&
cc -o date main.o B.o BSys.o