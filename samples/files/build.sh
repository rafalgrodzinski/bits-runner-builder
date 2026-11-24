#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

brb "${SCRIPT_DIR}/main.brc" "${SCRIPT_DIR}/../../lib/brc/BSys.brc" "${SCRIPT_DIR}/../../lib/brc/B/String.brc" "${SCRIPT_DIR}/../../lib/brc/B/FsNode.brc" "${SCRIPT_DIR}/../../lib/brc/B/File.brc" "${SCRIPT_DIR}/../../lib/brc/B/Dir.brc" "${SCRIPT_DIR}/../../lib/brc/B/Result.brc" &&
cc -o files main.o B.o BSys.o