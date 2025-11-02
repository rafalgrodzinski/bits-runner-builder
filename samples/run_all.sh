#!/bin/bash

SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"

function check {
    if [ $? -ne 0 ]; then
        exit 1
    fi
}

samples=(
    callback
    casts
    external_linkage
    fib
    fizz_buzz
    hello
    linked_list
    merge_sort
    multi_module
    primes
)

for sample in "${samples[@]}"; do
    echo "🐷 Building ${sample}" &&
    "${SCRIPT_DIR}/${sample}/build.sh" &&
    echo "🐷🐷 Running ${sample}" &&
    "./${sample}"
    check
    echo
done