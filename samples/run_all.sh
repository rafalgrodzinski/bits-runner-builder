#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

function check {
    if [ $? -ne 0 ]; then
        echo "⛔️ \"${1}\" Failed"
        exit 1
    fi
}

echo "🤖 Building brb..."
cmake -B "${SCRIPT_DIR}/../build" &&
cmake --build "${SCRIPT_DIR}/../build" --config RelWithDebInfo
check "Building brb"
echo

export PATH="${SCRIPT_DIR}/../build/:${PATH}"

cd "${SCRIPT_DIR}" > /dev/null
SAMPLES=`ls -d */ | cut -f1 -d'/'`
cd - > /dev/null

for SAMPLE in ${SAMPLES}; do
    echo "🐷 Building ${SAMPLE}" &&
    "${SCRIPT_DIR}/${SAMPLE}/build.sh" &&
    echo "🐷🐷 Running ${SAMPLE}" &&
    "./${SAMPLE}"
    check "${SAMPLE}"
    echo
done