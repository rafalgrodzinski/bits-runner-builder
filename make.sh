#!/bin/bash

cc -g -std=c++17 -lc++ -lllvm -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1 -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include src/*.cpp -o brb