#!/bin/bash

export CXX=clang++
export CC=clang

mkdir -v build
cd build/
cmake
make && make install

cd ..
./build_modules.sh
