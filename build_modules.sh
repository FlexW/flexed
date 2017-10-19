#!/bin/bash

export CXX=clang++
export CC=clang

mkdir build_modules
cd build_modules
cmake -DCMAKE_BUILD_TYPE=Debug ../src/module/
make && make install
