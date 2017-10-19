#!/bin/bash

export CXX=clang++
export CC=clang

mkdir -v analyze
cd analyze/
cmake ../
scan-build make

cd ..
./analyze_modules.sh
