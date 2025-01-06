#!/bin/bash
set -e
./build.sh
gdb ./build/pixelopolis -ex "run $@" -ex quit
eog ./out.png
