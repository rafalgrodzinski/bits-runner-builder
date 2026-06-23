#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

brb "${SCRIPT_DIR}/main.brc" "${SCRIPT_DIR}/Lib.brc" "${SCRIPT_DIR}/../../lib/brc/B/String.brc" "${SCRIPT_DIR}/../../lib/brc/BSys.brc" &&
cc -o interfaces main.o Lib.o B.o BSys.o