#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

function check {
    if [ $? -ne 0 ]; then
        echo "⛔️ ${1} failed"
        exit 1
    fi
}

# Setup Environment
export PATH="${SCRIPT_DIR}/../build/:${PATH}"

# Build brb
echo "🤖 Building brb..."
cmake -B "${SCRIPT_DIR}/../build" &&
cmake --build "${SCRIPT_DIR}/../build" --config RelWithDebInfo
check "Building brb"
echo

# Build and run samples
cd "${SCRIPT_DIR}" > /dev/null
SAMPLES=`ls -d */ | cut -f1 -d'/'`
cd - > /dev/null

for SAMPLE in ${SAMPLES}; do
    # Build sample
    echo "🐷 Building ${SAMPLE}" &&
    "${SCRIPT_DIR}/${SAMPLE}/build.sh" &&
    # Run sample
    echo "🐷🐷 Running ${SAMPLE}" &&
    "./${SAMPLE}"
    check "${SAMPLE}"

    echo
done