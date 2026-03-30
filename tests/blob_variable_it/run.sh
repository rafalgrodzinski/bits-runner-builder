#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"
source "${SCRIPT_DIR}/../lib.sh"

brb "${SCRIPT_DIR}/main.brc" &&
cc -o ${TEST_NAME} main.o
check

./${TEST_NAME}
[ ${?} = 255 ]
check_test ${TEST_NAME} ${?}