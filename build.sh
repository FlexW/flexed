#!/bin/bash

export CXX=clang++
export CC=clang

mkdir -v build
cd build/
cmake -DCMAKE_BUILD_TYPE=Debug ../
make && make install

cd ..
./build_modules.sh
