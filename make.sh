#!/bin/bash

IFLAGS="-I/usr/local/opt/llvm/include -I/opt/homebrew/opt/llvm/include"
LDFLAGS="-L/usr/local/opt/llvm/lib -L/opt/homebrew/opt/llvm/lib"

cc -g -std=c++17 -lc++ -lllvm -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1 ${LDFLAGS} ${IFLAGS} src/*.cpp -o brb