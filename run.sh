#!/bin/bash
set -e
./build.sh
./build/pixelopolis $@
eog ./out.png
