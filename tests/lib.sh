LIB_SCRIPT_PATH="$(readlink -f "${BASH_SOURCE}")"
LIB_SCRIPT_DIR="$(dirname "${SCRIPT_PATH}")"
PATH="${LIB_SCRIPT_DIR}/../build:${PATH}"
TEST_NAME=`basename "${SCRIPT_DIR}"`

function check {
    if [ $? -ne 0 ]; then
        exit 1
    fi
}

function check_test {
    if [ ${2} -eq 0 ]; then
        echo "✅ Test \"${1}\" Passed"
    else
        echo "⛔️ Test \"${1}\" Failed"
    fi
}