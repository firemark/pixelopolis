#!/bin/bash
set -e
mkdir -p build_wasm
cd build_wasm
#cmake .. -DCMAKE_BUILD_TYPE=Release -DWASM_ENABLED=1
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWASM_ENABLED=1

make
