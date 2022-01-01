#!/bin/bash
set -e
mkdir -p out
cd out
cmake ..
make
