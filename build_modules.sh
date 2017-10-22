#!/bin/bash

export CXX=clang++
export CC=clang

mkdir build_modules
cd build_modules
cmake ../src/module/
make && make install
