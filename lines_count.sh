#!/bin/bash
find . \( -name "*.h" -o -name "*.cpp" \) -print0 | xargs -0 wc -l
