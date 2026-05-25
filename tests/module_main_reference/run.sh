#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"
source "${SCRIPT_DIR}/../lib.sh"

brb "${SCRIPT_DIR}/main.brc" "${SCRIPT_DIR}/dummy.brc" &&
cc -o ${TEST_NAME} main.o dummy.o && 
./${TEST_NAME}

[ ${?} = 13 ]
check_test ${TEST_NAME} ${?}