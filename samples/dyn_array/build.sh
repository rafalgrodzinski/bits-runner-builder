#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

brb "${SCRIPT_DIR}/main.brc" "${SCRIPT_DIR}/../../lib/B/String.brc" "${SCRIPT_DIR}/../../lib/B/Array.brc" "${SCRIPT_DIR}/../../lib/B/BSys.brc" &&
cc -o dyn_array main.o B.o BSys.o