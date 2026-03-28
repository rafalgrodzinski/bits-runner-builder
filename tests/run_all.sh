#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"
source "${SCRIPT_DIR}/lib.sh"

echo "🤖 Building brb..."
cmake -B "${SCRIPT_DIR}/../build" &&
cmake --build "${SCRIPT_DIR}/../build" --config RelWithDebInfo
check
echo

echo "🤖 Running tests"

cd "${SCRIPT_DIR}" > /dev/null
TESTS=`ls -d */ | cut -f1 -d'/'`
cd - > /dev/null

for TEST in ${TESTS}; do
    echo
    echo "🤖 Running test \"${TEST}\"..."
    "${SCRIPT_DIR}/${TEST}/run.sh"
done
