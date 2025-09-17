#!/bin/bash
find . -path ./build -prune -o \( -name "*.h" -o -name "*.cpp" \) -print0 | xargs -0 wc -l
