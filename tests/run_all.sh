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
echo

export PATH="${SCRIPT_DIR}/../build/:${PATH}"

cd "${SCRIPT_DIR}" > /dev/null
TESTS=`ls -d */ | cut -f1 -d'/'`
cd - > /dev/null

PASSED_TESTS=0
FAILED_TESTS=0

for TEST in ${TESTS}; do
    echo "🤖 Running test \"${TEST}\"..."
    "${SCRIPT_DIR}/${TEST}/run.sh"
    if [ ${?} -eq 0 ]; then
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    echo
done

echo "✅ Passed tests: ${PASSED_TESTS}"
echo "⛔️ Failed tests: ${FAILED_TESTS}"